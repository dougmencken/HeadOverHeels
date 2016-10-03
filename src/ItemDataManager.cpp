#include "ItemDataManager.hpp"
#include "Exception.hpp"
#include "Ism.hpp"
#include <fstream>

namespace isomot
{

ItemDataManager::ItemDataManager(const std::string& fileName)
{
  this->fileName = fileName;
}

ItemDataManager::~ItemDataManager()
{
  std::for_each(itemData.begin(), itemData.end(), destroyItemData);
}

void ItemDataManager::load()
{
  // Carga el archivo XML especificado y almacena los datos XML en la lista
  try
  {
    std::auto_ptr<ixml::ItemsXML> itemsXML(ixml::items((isomot::sharePath() + fileName).c_str()));

    for(ixml::ItemsXML::item_const_iterator i = itemsXML->item().begin(); i != itemsXML->item().end(); ++i)
    {
      std::auto_ptr<ItemData> item(new ItemData);

      // Identificador único y exclusivo del elemento
      item->label = (*i).label();
      // Número de fotogramas en función de la dirección del elemento
      item->directionFrames = (*i).directionFrames();
      // Elemento ofensivo o inofensivo
      item->mortal = (*i).mortal();
      // Cada cuántos milisegundos cae el elemento
      item->weight = (*i).weight();
      // Cada cuántos milisegundos cambian los fotogramas de la secuencia de animación
      item->framesDelay = (*i).framesDelay();
      // Cada cuántos milisegundos se desplaza el elemento una unidad isométrica
      item->speed = (*i).speed();
      // Ruta a los gráficos del elemento
      item->bitmap = isomot::sharePath() + (*i).bitmap().file();
      // Anchura en píxeles de un fotograma
      item->frameWidth = (*i).bitmap().frameWidth();
      // Altura en píxeles de un fotograma
      item->frameHeight = (*i).bitmap().frameHeight();
      // Se almacenan los fotogramas en el vector motion siempre que el elemento no sea una puerta
      if(!(*i).door().present())
      {
        createBitmapFrames(item.get());
      }

      // El elemento puede no tener sombra
      if((*i).shadow().present())
      {
        // Ruta a la sombra del elemento
        item->shadow = isomot::sharePath() + (*i).shadow().get().file();
        // Anchura en píxeles de un fotograma de sombra
        item->shadowWidth = (*i).shadow().get().shadowWidth();
        // Altura en píxeles de un fotograma de sombra
        item->shadowHeight = (*i).shadow().get().shadowHeight();
        // Se almacenan los fotogramas en el vector shadows
        createShadowFrames(item.get());
      }

      // Sólo unos pocos elementos tienen fotogramas extra
      if((*i).extraFrames().present())
      {
        item->extraFrames = (*i).extraFrames().get();
      }

      // Secuencia de animación
      for(ixml::item::frame_const_iterator j = (*i).frame().begin(); j != (*i).frame().end(); ++j)
      {
        item->frames.push_back(*j);
      }

      // Si el elemento es una puerta, tiene tres parámetros que definen sus dimensiones
      if((*i).door().present())
      {
        // Una puerta en realidad son tres elementos distintos: dintel, jamba izquierda y jamba derecha
        ItemData lintel(*item);
        ItemData leftJamb(*item);
        ItemData rightJamb(*item);
        // Dimensiones de las partes de una puerta
        DoorMeasures dm;

        // Anchura espacial en el eje X de las tres partes de la puerta
        // Se almacenan en el siguiente orden: jamba izquierda, jamba derecha y dintel
        for(ixml::item::widthX_const_iterator j = (*i).widthX().begin(); j != (*i).widthX().end(); ++j)
        {
          if(dm.leftJambWidthX == 0)
          {
            leftJamb.widthX = dm.leftJambWidthX = *j;
          }
          else if(dm.rightJambWidthX == 0)
          {
            rightJamb.widthX = dm.rightJambWidthX = *j;
          }
          else if(dm.lintelWidthX == 0)
          {
            lintel.widthX = dm.lintelWidthX = *j;
          }
        }

        // Anchura espacial en el eje Y de las tres partes de la puerta
        // Se almacenan en el siguiente orden: jamba izquierda, jamba derecha y dintel
        for(ixml::item::widthY_const_iterator j = (*i).widthY().begin(); j != (*i).widthY().end(); ++j)
        {
          if(dm.leftJambWidthY == 0)
          {
            leftJamb.widthY = dm.leftJambWidthY = *j;
          }
          else if(dm.rightJambWidthY == 0)
          {
            rightJamb.widthY = dm.rightJambWidthY = *j;
          }
          else if(dm.lintelWidthY == 0)
          {
            lintel.widthY = dm.lintelWidthY = *j;
          }
        }

        // Altura espacial de las tres partes de la puerta
        // Se almacenan en el siguiente orden: jamba izquierda, jamba derecha y dintel
        for(ixml::item::height_const_iterator j = (*i).height().begin(); j != (*i).height().end(); ++j)
        {
          if(dm.leftJambHeight == 0)
          {
            leftJamb.height = dm.leftJambHeight = *j;
          }
          else if(dm.rightJambHeight == 0)
          {
            rightJamb.height = dm.rightJambHeight = *j;
          }
          else if(dm.lintelHeight == 0)
          {
            lintel.height = dm.lintelHeight = *j;
          }
        }

        // Se cargan el gráfico de la puerta al completo y se obtienen los gráficos de sus tres partes
        BITMAP* gfx = load_bitmap(item->bitmap.c_str(), 0);
        if(gfx == 0) throw "Bitmap " + item->bitmap + " not found.";

        // Creación de la jamba izquierda
        BITMAP* left = cutOutLeftJamb(gfx, dm, (*i).door().get());
        leftJamb.motion.push_back(left);
        this->itemData.push_back(leftJamb);

        // Creación de la jamba derecha
        BITMAP* right = cutOutRightJamb(gfx, dm, (*i).door().get());
        rightJamb.label += 1;
        rightJamb.motion.push_back(right);
        this->itemData.push_back(rightJamb);

        // Creación del dintel
        BITMAP* top = cutOutLintel(gfx, dm, (*i).door().get());
        lintel.label += 2;
        lintel.motion.push_back(top);
        this->itemData.push_back(lintel);

        // La imagen original no se volverá a utilizar
        destroy_bitmap(gfx);
      }
      else
      {
        // Anchura en el eje X en unidades isométricas del elemento
        item->widthX = *((*i).widthX()).begin();
        // Anchura en el eje Y en unidades isométricas del elemento
        item->widthY = *((*i).widthY()).begin();
        // Anchura en el eje Z en unidades isométricas del elemento; su altura
        item->height = *((*i).height()).begin();

        // Los datos del elemento simple se incorporan a la lista
        this->itemData.push_back(*item.get());
      }
    }
  }
  catch(const xml_schema::exception& e)
  {
    std::cerr << e << std::endl;
  }
  catch(const Exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

ItemData* ItemDataManager::find(const short label)
{
  std::list<ItemData>::iterator i = std::find_if(itemData.begin(), itemData.end(), std::bind2nd(EqualItemData(), label));
  ItemData* data = (i != itemData.end() ? static_cast<ItemData*>(&(*i)) : 0);

  return data;
}

ItemDataManager::DoorMeasures::DoorMeasures()
{
  lintelWidthX = lintelWidthY = lintelHeight = 0;
  leftJambWidthX = leftJambWidthY = leftJambHeight = 0;
  rightJambWidthX = rightJambWidthY = rightJambHeight = 0;
}

ItemData* ItemDataManager::createBitmapFrames(ItemData* itemData)
{
  try
  {
    // Datos necesarios
    if(itemData->bitmap.empty() || itemData->frameWidth == 0 || itemData->frameHeight == 0)
    {
      throw "Insufficient data for ItemDataManager::createBitmapFrames.";
    }

    // Se cargan los gráficos del elemento y si es necesario se crean los distintos fotogramas
    BITMAP* gfx = load_bitmap(itemData->bitmap.c_str(), 0);
    if(gfx == 0) throw "Bitmap " + itemData->bitmap + " not found.";

    // Trocea la imagen en los distintos fotogramas y se almacenan en el vector
    for(int y = 0; y < gfx->h / itemData->frameHeight; y++)
    {
      for(int x = 0; x < gfx->w / itemData->frameWidth; x++)
      {
        BITMAP* frame = create_bitmap_ex(32, itemData->frameWidth, itemData->frameHeight);
        blit(gfx, frame, x * itemData->frameWidth, y * itemData->frameHeight, 0, 0, frame->w, frame->h);
        itemData->motion.push_back(frame);
      }
    }

    // La imagen original no se volverá a utilizar
    destroy_bitmap(gfx);
  }
  catch(const Exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 0;
  }

  return itemData;
}

ItemData* ItemDataManager::createShadowFrames(ItemData* itemData)
{
  try
  {
    // Datos necesarios
    if(itemData->shadow.empty() || itemData->shadowWidth == 0 || itemData->shadowHeight == 0)
    {
      throw "Insufficient data for ItemDataManager::createShadowFrames.";
    }

    // Se cargan los gráficos del elemento y si es necesario se crean los distintos fotogramas
    BITMAP* gfx = load_bitmap(itemData->shadow.c_str(), 0);
    if(gfx == 0) throw "Bitmap " + itemData->shadow + " not found.";

    // Trocea la imagen en los distintos fotogramas y se almacenan en el vector
    for(int y = 0; y < gfx->h / itemData->shadowHeight; y++)
    {
      for(int x = 0; x < gfx->w / itemData->shadowWidth; x++)
      {
        BITMAP* frame = create_bitmap_ex(32, itemData->shadowWidth, itemData->shadowHeight);
        blit(gfx, frame, x * itemData->shadowWidth, y * itemData->shadowHeight, 0, 0, frame->w, frame->h);
        itemData->shadows.push_back(frame);
      }
    }

    // La imagen original no se volverá a utilizar
    destroy_bitmap(gfx);
  }
  catch(const Exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 0;
  }

  return itemData;
}

BITMAP* ItemDataManager::cutOutLintel(BITMAP* door, const DoorMeasures& dm, const ixml::door::value type)
{
  bool ns = (type == ixml::door::north || type == ixml::door::south);

  // Parte superior de la puerta
  BITMAP* top = create_bitmap_ex(32, (dm.lintelWidthX << 1) + (dm.lintelWidthY << 1),
                                      dm.lintelHeight + dm.lintelWidthY + dm.lintelWidthX);
  // El color de fondo es magenta
  clear_to_color(top, makecol(255, 0, 255));

  // Copia la zona a recortar si la puerta está orientada al norte o al sur
  if(ns)
  {
    // Copia la zona rectangular
    blit(door, top, 0, 0, 0, 0, top->w, dm.lintelHeight + dm.lintelWidthX);

    int delta = top->w - 1;
    int noPixelIndex = top->w - ((dm.rightJambWidthX + dm.rightJambWidthY) << 1) + 1;
    int yStart = noPixelIndex;
    int yEnd = noPixelIndex - 1;

    acquire_bitmap(top);

    for(int x = dm.lintelHeight + dm.lintelWidthX; x < top->h; x++)
    {
      for(int y = delta; y >= 0; y--)
      {
        if(x != dm.lintelHeight + dm.lintelWidthX && y == noPixelIndex)
        {
          if(noPixelIndex > yEnd)
          {
            noPixelIndex--;
          }
          else
          {
            yStart += 2;
            noPixelIndex = yStart;
          }
        }
        else if(x < dm.lintelHeight + (dm.lintelWidthX << 1) || y < yEnd)
        {
          ((int*)top->line[x])[y] = ((int*)door->line[x])[y];
        }
      }

      delta -= 2;
    }

    release_bitmap(top);
  }
  else
  // Copia la zona a recortar si la puerta está orientada al este o al oeste
  {
    // Copia la zona rectangular
    blit(door, top, 0, 0, 0, 0, top->w, dm.lintelHeight + dm.lintelWidthY);

    int delta = 0;
    int noPixelIndex = ((dm.leftJambWidthX + dm.leftJambWidthY) << 1) - 2;
    int yStart = noPixelIndex;
    int yEnd = noPixelIndex + 1;

    acquire_bitmap(top);

    for(int x = dm.lintelHeight + dm.lintelWidthY; x < top->h; x++)
    {
      for(int y = delta; y < top->w; y++)
      {
        if(x != dm.lintelHeight + dm.lintelWidthY && y == noPixelIndex)
        {
          if(noPixelIndex < yEnd)
          {
            noPixelIndex++;
          }
          else
          {
            yStart -= 2;
            noPixelIndex = yStart;
          }
        }
        else if(x < dm.lintelHeight + (dm.lintelWidthY << 1) || y > yEnd)
        {
          ((int*)top->line[x])[y] = ((int*)door->line[x])[y];
        }
      }

      delta += 2;
    }

    release_bitmap(top);
  }

  return top;
}

BITMAP* ItemDataManager::cutOutLeftJamb(BITMAP* door, const DoorMeasures& dm, const ixml::door::value type)
{
  bool ns = (type == ixml::door::north || type == ixml::door::south);
  int fixWidth = (ns ? 7 : 0);
  int fixY = (ns ? -1 : 0);

  BITMAP* left = create_bitmap_ex(32, (dm.leftJambWidthX << 1) + fixWidth + (dm.leftJambWidthY << 1),
                                       dm.leftJambHeight + dm.leftJambWidthY + dm.leftJambWidthX);

  // El color de fondo es magenta
  clear_to_color(left, makecol(255, 0, 255));
  // Copia la zona rectangular
  blit(door, left, fixY, dm.lintelHeight + dm.lintelWidthY - dm.leftJambWidthY + fixY, 0, 0, left->w, left->h);

  return left;
}

BITMAP* ItemDataManager::cutOutRightJamb(BITMAP* door, const DoorMeasures& dm, const ixml::door::value type)
{
  bool ns = (type == ixml::door::north || type == ixml::door::south);
  int fixWidth = (ns ? 0 : 7);
  int fixY = (ns ? 0 : -2);

  BITMAP* right = create_bitmap_ex(32, (dm.rightJambWidthX << 1) + fixWidth + (dm.rightJambWidthY << 1),
                                        dm.rightJambHeight + dm.rightJambWidthY + dm.rightJambWidthX);

  // El color de fondo es magenta
  clear_to_color(right, makecol(255, 0, 255));
  // Copia la zona rectangular
  blit(door, right, door->w - right->w, dm.lintelHeight + dm.lintelWidthX - dm.rightJambWidthY + fixY, 0, 0, right->w, right->h);

  return right;
}

void ItemDataManager::destroyItemData(ItemData& itemData)
{
  itemData.bitmap.clear();
  itemData.shadow.clear();
  itemData.frames.clear();
  std::for_each(itemData.motion.begin(), itemData.motion.end(), destroy_bitmap);
  std::for_each(itemData.shadows.begin(), itemData.shadows.end(), destroy_bitmap);
}

bool EqualItemData::operator()(const ItemData& itemData, short label) const
{
  return (itemData.label == label);
}

}
