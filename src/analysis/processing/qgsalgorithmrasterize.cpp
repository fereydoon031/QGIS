/***************************************************************************
  qgsalgorithmrasterize.cpp - QgsRasterizeAlgorithm

 ---------------------
 Original implementation in Python:

 begin                : 2016-10-05
 copyright            : (C) 2016 by OPENGIS.ch
 email                : matthias@opengis.ch

 C++ port:

 begin                : 20.11.2019
 copyright            : (C) 2019 by Alessandro Pasotti
 email                : elpaso at itopen dot it
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsalgorithmrasterize.h"
#include "qgsprocessingparameters.h"
#include "qgsmapthemecollection.h"
#include "qgsrasterfilewriter.h"
#include "qgsmaprenderercustompainterjob.h"
#include "gdal.h"
#include "qgsgdalutils.h"

///@cond PRIVATE

QString QgsRasterizeAlgorithm::name() const
{
  return QStringLiteral( "rasterize" );
}

QString QgsRasterizeAlgorithm::displayName() const
{
  return QObject::tr( "Convert map to raster" );
}

QStringList QgsRasterizeAlgorithm::tags() const
{
  return QObject::tr( "layer,raster,convert,file,map themes,tiles,render" ).split( ',' );
}

QString QgsRasterizeAlgorithm::group() const
{
  return QObject::tr( "Raster tools" );
}

QString QgsRasterizeAlgorithm::groupId() const
{
  return QStringLiteral( "rastertools" );
}

void QgsRasterizeAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterExtent(
                  QStringLiteral( "EXTENT" ),
                  QObject::tr( "Minimum extent to render" ) ) );
  addParameter( new QgsProcessingParameterNumber(
                  QStringLiteral( "EXTENT_BUFFER" ),
                  QObject::tr( "Buffer around tiles in map units" ),
                  QgsProcessingParameterNumber::Type::Double,
                  0,
                  true,
                  0 ) );
  addParameter( new QgsProcessingParameterNumber(
                  QStringLiteral( "TILE_SIZE" ),
                  QObject::tr( "Tile size" ),
                  QgsProcessingParameterNumber::Type::Integer,
                  1024,
                  false,
                  64 ) );
  addParameter( new QgsProcessingParameterNumber(
                  QStringLiteral( "MAP_UNITS_PER_PIXEL" ),
                  QObject::tr( "Map units per pixel" ),
                  QgsProcessingParameterNumber::Type::Double,
                  100,
                  true,
                  0 ) );
  addParameter( new QgsProcessingParameterBoolean(
                  QStringLiteral( "MAKE_BACKGROUND_TRANSPARENT" ),
                  QObject::tr( "Make background transparent" ),
                  false ) );
  QStringList mapThemes { QgsProject::instance()->mapThemeCollection()->mapThemes() };
  mapThemes.prepend( QString() );
  addParameter( new QgsProcessingParameterEnum(
                  QStringLiteral( "MAP_THEME" ),
                  QObject::tr( "Map theme to render" ),
                  mapThemes,
                  false,
                  QString(),
                  true
                ) );
  QList<QgsMapLayer *> projectLayers { QgsProject::instance()->mapLayers().values() };
  addParameter( new QgsProcessingParameterMultipleLayers(
                  QStringLiteral( "LAYERS" ),
                  QObject::tr( "Layers to render" ),
                  QgsProcessing::TypeMapLayer,
                  QVariant(),
                  true
                ) );
  addParameter( new QgsProcessingParameterRasterDestination(
                  QStringLiteral( "OUTPUT" ),
                  QObject::tr( "Output layer" ) ) );

}

QString QgsRasterizeAlgorithm::shortDescription() const
{
  return QObject::tr( R"(This algorithm rasterizes map canvas content.
                      A map theme can be selected to render a predetermined set of layers with a defined style for each layer.
                      Alternatively, a set of layers layer can be selected if no map theme is set.
                      If neither map theme nor layer is set all the current project layers will be
                      rendered.
                      The minimum extent entered will internally be extended to be a multiple of the tile size.)" );
}

QString QgsRasterizeAlgorithm::shortHelpString() const
{
  return QObject::tr( R"(This algorithm renders the map canvas to a raster file.
                      It's possible to choose the following parameters:
                          - Map theme to render
                          - Layers to render
                          - The minimum extent to render
                          - The tile size
                          - Map unit per pixel
                          - The output (can be saved to a file or to a temporary file and
                            automatically opened as layer in qgis)
                      )" );
}

QgsRasterizeAlgorithm *QgsRasterizeAlgorithm::createInstance() const
{
  return new QgsRasterizeAlgorithm();
}


QVariantMap QgsRasterizeAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{

  const QString mapTheme { parameterAsString( parameters, QStringLiteral( "MAP_THEME" ), context ) };
  const QList<QgsMapLayer *> mapLayers { parameterAsLayerList( parameters, QStringLiteral( "LAYERS" ), context ) };
  const QgsRectangle extent { parameterAsExtent( parameters, QStringLiteral( "EXTENT" ), context, context.project()->crs() ) };
  const int tileSize { parameterAsInt( parameters, QStringLiteral( "TILE_SIZE" ), context ) };
  const bool transparent { parameterAsBool( parameters, QStringLiteral( "MAKE_BACKGROUND_TRANSPARENT" ), context ) };
  const double mapUnitsPerPixel { parameterAsDouble( parameters, QStringLiteral( "MAP_UNITS_PER_PIXEL" ), context ) };
  const double extentBuffer { parameterAsDouble( parameters, QStringLiteral( "EXTENT_BUFFER" ), context ) };
  const QString outputLayerFileName { parameterAsOutputLayer( parameters, QStringLiteral( "OUTPUT" ), context )};

  int xTileCount { static_cast<int>( ceil( extent.width() / mapUnitsPerPixel / tileSize ) )};
  int yTileCount { static_cast<int>( ceil( extent.height() / mapUnitsPerPixel / tileSize ) )};
  int width { xTileCount * tileSize };
  int height { yTileCount * tileSize };
  int nBands { transparent ? 4 : 3 };

  const QString driverName { QgsRasterFileWriter::driverForExtension( QFileInfo( outputLayerFileName ).suffix() ) };
  if ( driverName.isEmpty() )
  {
    throw QgsProcessingException( QObject::tr( "Invalid output raster format" ) );
  }

  GDALDriverH hOutputFileDriver = GDALGetDriverByName( driverName.toLocal8Bit().constData() );
  if ( !hOutputFileDriver )
  {
    throw QgsProcessingException( QObject::tr( "Error creating GDAL driver" ) );
  }

  gdal::dataset_unique_ptr hOutputDataset( GDALCreate( hOutputFileDriver, outputLayerFileName.toLocal8Bit().constData(), width, height, nBands, GDALDataType::GDT_Byte, nullptr ) );
  if ( !hOutputDataset )
  {
    throw QgsProcessingException( QObject::tr( "Error creating GDAL output layer" ) );
  }

  GDALSetProjection( hOutputDataset.get(), context.project()->crs().toWkt().toLatin1().constData() );
  double geoTransform[6];
  geoTransform[0] = extent.xMinimum();
  geoTransform[1] = mapUnitsPerPixel;
  geoTransform[2] = 0;
  geoTransform[3] = extent.yMaximum();
  geoTransform[4] = 0;
  geoTransform[5] = - mapUnitsPerPixel;
  GDALSetGeoTransform( hOutputDataset.get(), geoTransform );

  int red = context.project()->readNumEntry( QStringLiteral( "Gui" ), "/CanvasColorRedPart", 255 );
  int green = context.project()->readNumEntry( QStringLiteral( "Gui" ), "/CanvasColorGreenPart", 255 );
  int blue = context.project()->readNumEntry( QStringLiteral( "Gui" ), "/CanvasColorBluePart", 255 );

  QColor bgColor;
  if ( transparent )
  {
    bgColor = QColor( red, green, blue, 0 );
  }
  else
  {
    bgColor = QColor( red, green, blue );
  }

  QgsMapSettings mapSettings;
  mapSettings.setOutputImageFormat( QImage::Format_ARGB32 );
  mapSettings.setDestinationCrs( context.project()->crs() );
  mapSettings.setFlag( QgsMapSettings::Antialiasing, true );
  mapSettings.setFlag( QgsMapSettings::RenderMapTile, true );
  mapSettings.setFlag( QgsMapSettings::UseAdvancedEffects, true );
  mapSettings.setTransformContext( context.transformContext() );
  mapSettings.setExtentBuffer( extentBuffer );
  mapSettings.setBackgroundColor( bgColor );

  // Now get layers
  if ( ! mapTheme.isEmpty() && context.project()->mapThemeCollection()->hasMapTheme( mapTheme ) )
  {
    mapSettings.setLayers( context.project()->mapThemeCollection()->mapThemeVisibleLayers( mapTheme ) );
    mapSettings.setLayerStyleOverrides( context.project()->mapThemeCollection( )->mapThemeStyleOverrides( mapTheme ) );
  }
  else if ( ! mapLayers.isEmpty() )
  {
    mapSettings.setLayers( mapLayers );
  }

  // Still no layers? Get them all from the project
  if ( mapSettings.layers().isEmpty() )
  {
    mapSettings.setLayers( context.project()->mapLayers().values() );
  }

  QImage image { tileSize, tileSize, QImage::Format::Format_ARGB32 };
  mapSettings.setOutputDpi( image.logicalDpiX() );
  mapSettings.setOutputSize( image.size() );
  QPainter painter { &image };

  // Start rendering
  const double extentRatio { mapUnitsPerPixel * tileSize };
  const int numTiles { xTileCount * yTileCount };
  const QString fileExtension { QFileInfo( outputLayerFileName ).suffix() };

  // Custom deleter for CPL allocation
  struct CPLDelete
  {
    void operator()( uint8_t *ptr ) const
    {
      CPLFree( ptr );
    }
  };

  std::unique_ptr<uint8_t, CPLDelete> buffer( static_cast< uint8_t * >( CPLMalloc( sizeof( uint8_t ) * static_cast<size_t>( tileSize * tileSize * nBands ) ) ) );
  feedback->setProgress( 0 );

  for ( int x = 0; x < xTileCount; ++x )
  {
    for ( int y = 0; y < yTileCount; ++y )
    {
      if ( feedback->isCanceled() )
      {
        return {};
      }
      image.fill( transparent ? bgColor.rgba() : bgColor.rgb() );
      mapSettings.setExtent( QgsRectangle(
                               extent.xMinimum() + x * extentRatio,
                               extent.yMaximum() - ( y + 1 ) * extentRatio,
                               extent.xMinimum() + ( x + 1 ) * extentRatio,
                               extent.yMaximum() - y * extentRatio
                             ) );
      QgsMapRendererCustomPainterJob job( mapSettings, &painter );
      job.start();
      job.waitForFinished();

      gdal::dataset_unique_ptr hIntermediateDataset( QgsGdalUtils::imageToMemoryDataset( image ) );
      if ( !hIntermediateDataset )
      {
        throw QgsProcessingException( QStringLiteral( "Error reading tiles from the temporary image" ) );
      }

      const int xOffset { x * tileSize };
      const int yOffset { y * tileSize };

      CPLErr err = GDALDatasetRasterIO( hIntermediateDataset.get(),
                                        GF_Read, 0, 0, tileSize, tileSize,
                                        buffer.get(),
                                        tileSize, tileSize, GDT_Byte, nBands, nullptr, 0, 0, 0 );
      if ( err != CE_None )
      {
        throw QgsProcessingException( QStringLiteral( "Error reading intermediate raster" ) );
      }

      err = GDALDatasetRasterIO( hOutputDataset.get(),
                                 GF_Write, xOffset, yOffset, tileSize, tileSize,
                                 buffer.get(),
                                 tileSize, tileSize, GDT_Byte, nBands, nullptr, 0, 0, 0 );
      if ( err != CE_None )
      {
        throw QgsProcessingException( QStringLiteral( "Error writing output raster" ) );
      }

      feedback->setProgress( static_cast<double>( x * yTileCount + y ) / numTiles * 100.0 );
    }
  }

  return { { QStringLiteral( "OUTPUT" ), outputLayerFileName } };
}

///@endcond
