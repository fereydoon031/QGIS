/***************************************************************************
                         qgsnativealgorithms.cpp
                         ---------------------
    begin                : April 2017
    copyright            : (C) 2017 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsnativealgorithms.h"
#include "qgsalgorithmaddincrementalfield.h"
#include "qgsalgorithmaddxyfields.h"
#include "qgsalgorithmarraytranslatedfeatures.h"
#include "qgsalgorithmaspect.h"
#include "qgsalgorithmassignprojection.h"
#include "qgsalgorithmattributeindex.h"
#include "qgsalgorithmboundary.h"
#include "qgsalgorithmboundingbox.h"
#include "qgsalgorithmbuffer.h"
#include "qgsalgorithmcalculateoverlaps.h"
#include "qgsalgorithmcategorizeusingstyle.h"
#include "qgsalgorithmcentroid.h"
#include "qgsalgorithmclip.h"
#include "qgsalgorithmconstantraster.h"
#include "qgsalgorithmconvexhull.h"
#include "qgsalgorithmdbscanclustering.h"
#include "qgsalgorithmdensifygeometriesbycount.h"
#include "qgsalgorithmdensifygeometriesbyinterval.h"
#include "qgsalgorithmdifference.h"
#include "qgsalgorithmdissolve.h"
#include "qgsalgorithmdrape.h"
#include "qgsalgorithmdropgeometry.h"
#include "qgsalgorithmdropmzvalues.h"
#include "qgsalgorithmexplode.h"
#include "qgsalgorithmexplodehstore.h"
#include "qgsalgorithmextendlines.h"
#include "qgsalgorithmextenttolayer.h"
#include "qgsalgorithmextractbinary.h"
#include "qgsalgorithmextractbyattribute.h"
#include "qgsalgorithmextractbyexpression.h"
#include "qgsalgorithmextractbyextent.h"
#include "qgsalgorithmextractbylocation.h"
#include "qgsalgorithmextractlayoutmapextent.h"
#include "qgsalgorithmextractvertices.h"
#include "qgsalgorithmextractzmvalues.h"
#include "qgsalgorithmfiledownloader.h"
#include "qgsalgorithmfilter.h"
#include "qgsalgorithmfiltervertices.h"
#include "qgsalgorithmfixgeometries.h"
#include "qgsalgorithmforcerhr.h"
#include "qgsalgorithmfuzzifyraster.h"
#include "qgsalgorithmgrid.h"
#include "qgsalgorithmhillshade.h"
#include "qgsalgorithmjoinbyattribute.h"
#include "qgsalgorithmjoinbynearest.h"
#include "qgsalgorithmjoinwithlines.h"
#include "qgsalgorithmimportphotos.h"
#include "qgsalgorithminterpolatepoint.h"
#include "qgsalgorithmintersection.h"
#include "qgsalgorithmkmeansclustering.h"
#include "qgsalgorithmlineintersection.h"
#include "qgsalgorithmlinesubstring.h"
#include "qgsalgorithmloadlayer.h"
#include "qgsalgorithmmeancoordinates.h"
#include "qgsalgorithmmergelines.h"
#include "qgsalgorithmmergevector.h"
#include "qgsalgorithmminimumenclosingcircle.h"
#include "qgsalgorithmmultiparttosinglepart.h"
#include "qgsalgorithmmultiringconstantbuffer.h"
#include "qgsalgorithmoffsetlines.h"
#include "qgsalgorithmorderbyexpression.h"
#include "qgsalgorithmorientedminimumboundingbox.h"
#include "qgsalgorithmorthogonalize.h"
#include "qgsalgorithmpackage.h"
#include "qgsalgorithmarrayoffsetlines.h"
#include "qgsalgorithmpointsinpolygon.h"
#include "qgsalgorithmpointonsurface.h"
#include "qgsalgorithmpointtolayer.h"
#include "qgsalgorithmpointsalonggeometry.h"
#include "qgsalgorithmpointslayerfromtable.h"
#include "qgsalgorithmprojectpointcartesian.h"
#include "qgsalgorithmpromotetomultipart.h"
#include "qgsalgorithmrandompointsextent.h"
#include "qgsalgorithmrasterlayeruniquevalues.h"
#include "qgsalgorithmrasterlogicalop.h"
#include "qgsalgorithmrasterize.h"
#include "qgsalgorithmrastersurfacevolume.h"
#include "qgsalgorithmrasterzonalstats.h"
#include "qgsalgorithmreclassifybylayer.h"
#include "qgsalgorithmremoveduplicatesbyattribute.h"
#include "qgsalgorithmremoveduplicatevertices.h"
#include "qgsalgorithmremoveholes.h"
#include "qgsalgorithmremovenullgeometry.h"
#include "qgsalgorithmrenamelayer.h"
#include "qgsalgorithmreverselinedirection.h"
#include "qgsalgorithmrotate.h"
#include "qgsalgorithmruggedness.h"
#include "qgsalgorithmsaveselectedfeatures.h"
#include "qgsalgorithmsegmentize.h"
#include "qgsalgorithmserviceareafromlayer.h"
#include "qgsalgorithmserviceareafrompoint.h"
#include "qgsalgorithmsetmvalue.h"
#include "qgsalgorithmsetzvalue.h"
#include "qgsalgorithmshortestpathlayertopoint.h"
#include "qgsalgorithmshortestpathpointtolayer.h"
#include "qgsalgorithmshortestpathpointtopoint.h"
#include "qgsalgorithmsimplify.h"
#include "qgsalgorithmsinglesidedbuffer.h"
#include "qgsalgorithmslope.h"
#include "qgsalgorithmsmooth.h"
#include "qgsalgorithmsnaptogrid.h"
#include "qgsalgorithmspatialindex.h"
#include "qgsalgorithmsplitlineantimeridian.h"
#include "qgsalgorithmsplitlinesbylength.h"
#include "qgsalgorithmsplitwithlines.h"
#include "qgsalgorithmsplitfeaturesbyattributecharacter.h"
#include "qgsalgorithmstringconcatenation.h"
#include "qgsalgorithmsubdivide.h"
#include "qgsalgorithmswapxy.h"
#include "qgsalgorithmsymmetricaldifference.h"
#include "qgsalgorithmtaperedbuffer.h"
#include "qgsalgorithmtransect.h"
#include "qgsalgorithmtransform.h"
#include "qgsalgorithmtranslate.h"
#include "qgsalgorithmunion.h"
#include "qgsalgorithmuniquevalueindex.h"
#include "qgsalgorithmvectorize.h"
#include "qgsalgorithmwedgebuffers.h"
#include "qgsalgorithmzonalhistogram.h"
#include "qgsalgorithmpolygonstolines.h"
#include "qgsbookmarkalgorithms.h"
#include "qgsprojectstylealgorithms.h"
#include "qgsstylealgorithms.h"

///@cond PRIVATE

QgsNativeAlgorithms::QgsNativeAlgorithms( QObject *parent )
  : QgsProcessingProvider( parent )
{}

QIcon QgsNativeAlgorithms::icon() const
{
  return QgsApplication::getThemeIcon( QStringLiteral( "/providerQgis.svg" ) );
}

QString QgsNativeAlgorithms::svgIconPath() const
{
  return QgsApplication::iconPath( QStringLiteral( "providerQgis.svg" ) );
}

QString QgsNativeAlgorithms::id() const
{
  return QStringLiteral( "native" );
}

QString QgsNativeAlgorithms::helpId() const
{
  return QStringLiteral( "qgis" );
}

QString QgsNativeAlgorithms::name() const
{
  return tr( "QGIS (native c++)" );
}

bool QgsNativeAlgorithms::supportsNonFileBasedOutput() const
{
  return true;
}

void QgsNativeAlgorithms::loadAlgorithms()
{
  addAlgorithm( new QgsAddIncrementalFieldAlgorithm() );
  addAlgorithm( new QgsAddXYFieldsAlgorithm() );
  addAlgorithm( new QgsAddUniqueValueIndexAlgorithm() );
  addAlgorithm( new QgsArrayTranslatedFeaturesAlgorithm() );
  addAlgorithm( new QgsAspectAlgorithm() );
  addAlgorithm( new QgsAssignProjectionAlgorithm() );
  addAlgorithm( new QgsAttributeIndexAlgorithm() );
  addAlgorithm( new QgsBookmarksToLayerAlgorithm() );
  addAlgorithm( new QgsBoundaryAlgorithm() );
  addAlgorithm( new QgsBoundingBoxAlgorithm() );
  addAlgorithm( new QgsBufferAlgorithm() );
  addAlgorithm( new QgsCalculateVectorOverlapsAlgorithm() );
  addAlgorithm( new QgsCategorizeUsingStyleAlgorithm() );
  addAlgorithm( new QgsCentroidAlgorithm() );
  addAlgorithm( new QgsClipAlgorithm() );
  addAlgorithm( new QgsCollectAlgorithm() );
  addAlgorithm( new QgsCombineStylesAlgorithm() );
  addAlgorithm( new QgsConstantRasterAlgorithm() );
  addAlgorithm( new QgsConvexHullAlgorithm() );
  addAlgorithm( new QgsDbscanClusteringAlgorithm() );
  addAlgorithm( new QgsDifferenceAlgorithm() );
  addAlgorithm( new QgsDissolveAlgorithm() );
  addAlgorithm( new QgsDrapeToMAlgorithm() );
  addAlgorithm( new QgsDrapeToZAlgorithm() );
  addAlgorithm( new QgsDropGeometryAlgorithm() );
  addAlgorithm( new QgsDropMZValuesAlgorithm() );
  addAlgorithm( new QgsExplodeAlgorithm() );
  addAlgorithm( new QgsExplodeHstoreAlgorithm() );
  addAlgorithm( new QgsExtendLinesAlgorithm() );
  addAlgorithm( new QgsExtentToLayerAlgorithm() );
  addAlgorithm( new QgsExtractBinaryFieldAlgorithm() );
  addAlgorithm( new QgsExtractByAttributeAlgorithm() );
  addAlgorithm( new QgsExtractByExpressionAlgorithm() );
  addAlgorithm( new QgsExtractByExtentAlgorithm() );
  addAlgorithm( new QgsExtractByLocationAlgorithm() );
  addAlgorithm( new QgsExtractMValuesAlgorithm() );
  addAlgorithm( new QgsExtractVerticesAlgorithm() );
  addAlgorithm( new QgsExtractZValuesAlgorithm() );
  addAlgorithm( new QgsFileDownloaderAlgorithm() );
  addAlgorithm( new QgsFilterAlgorithm() );
  addAlgorithm( new QgsFilterVerticesByM() );
  addAlgorithm( new QgsFilterVerticesByZ() );
  addAlgorithm( new QgsFixGeometriesAlgorithm() );
  addAlgorithm( new QgsForceRHRAlgorithm() );
  addAlgorithm( new QgsFuzzifyRasterLinearMembershipAlgorithm() );
  addAlgorithm( new QgsFuzzifyRasterPowerMembershipAlgorithm() );
  addAlgorithm( new QgsFuzzifyRasterLargeMembershipAlgorithm() );
  addAlgorithm( new QgsFuzzifyRasterSmallMembershipAlgorithm() );
  addAlgorithm( new QgsFuzzifyRasterGaussianMembershipAlgorithm() );
  addAlgorithm( new QgsFuzzifyRasterNearMembershipAlgorithm() );
  addAlgorithm( new QgsGridAlgorithm() );
  addAlgorithm( new QgsHillshadeAlgorithm() );
  addAlgorithm( new QgsImportPhotosAlgorithm() );
  addAlgorithm( new QgsInterpolatePointAlgorithm() );
  addAlgorithm( new QgsIntersectionAlgorithm() );
  addAlgorithm( new QgsJoinByAttributeAlgorithm() );
  addAlgorithm( new QgsJoinByNearestAlgorithm() );
  addAlgorithm( new QgsJoinWithLinesAlgorithm() );
  addAlgorithm( new QgsKMeansClusteringAlgorithm() );
  addAlgorithm( new QgsLayerToBookmarksAlgorithm() );
  addAlgorithm( new QgsLayoutMapExtentToLayerAlgorithm() );
  addAlgorithm( new QgsLineIntersectionAlgorithm() );
  addAlgorithm( new QgsLineSubstringAlgorithm() );
  addAlgorithm( new QgsLoadLayerAlgorithm() );
  addAlgorithm( new QgsMeanCoordinatesAlgorithm() );
  addAlgorithm( new QgsMergeLinesAlgorithm() );
  addAlgorithm( new QgsMergeVectorAlgorithm() );
  addAlgorithm( new QgsMinimumEnclosingCircleAlgorithm() );
  addAlgorithm( new QgsMultipartToSinglepartAlgorithm() );
  addAlgorithm( new QgsMultiRingConstantBufferAlgorithm() );
  addAlgorithm( new QgsOffsetLinesAlgorithm() );
  addAlgorithm( new QgsOrderByExpressionAlgorithm() );
  addAlgorithm( new QgsOrientedMinimumBoundingBoxAlgorithm() );
  addAlgorithm( new QgsOrthogonalizeAlgorithm() );
  addAlgorithm( new QgsPackageAlgorithm() );
  addAlgorithm( new QgsCreateArrayOffsetLinesAlgorithm() );
  addAlgorithm( new QgsPointsInPolygonAlgorithm() );
  addAlgorithm( new QgsPointOnSurfaceAlgorithm() );
  addAlgorithm( new QgsPointToLayerAlgorithm() );
  addAlgorithm( new QgsPointsAlongGeometryAlgorithm() );
  addAlgorithm( new QgsPointsLayerFromTableAlgorithm() );
  addAlgorithm( new QgsProjectPointCartesianAlgorithm() );
  addAlgorithm( new QgsPromoteToMultipartAlgorithm() );
  addAlgorithm( new QgsRandomPointsExtentAlgorithm() );
  addAlgorithm( new QgsRasterLayerUniqueValuesReportAlgorithm() );
  addAlgorithm( new QgsRasterLayerZonalStatsAlgorithm() );
  addAlgorithm( new QgsRasterLogicalAndAlgorithm() );
  addAlgorithm( new QgsRasterLogicalOrAlgorithm() );
  addAlgorithm( new QgsRasterizeAlgorithm() );
  addAlgorithm( new QgsRasterPixelsToPointsAlgorithm() );
  addAlgorithm( new QgsRasterPixelsToPolygonsAlgorithm() );
  addAlgorithm( new QgsRasterSurfaceVolumeAlgorithm() );
  addAlgorithm( new QgsAlgorithmRemoveDuplicateVertices() );
  addAlgorithm( new QgsReclassifyByLayerAlgorithm() );
  addAlgorithm( new QgsReclassifyByTableAlgorithm() );
  addAlgorithm( new QgsRemoveDuplicatesByAttributeAlgorithm() );
  addAlgorithm( new QgsRemoveHolesAlgorithm() );
  addAlgorithm( new QgsRemoveNullGeometryAlgorithm() );
  addAlgorithm( new QgsRenameLayerAlgorithm() );
  addAlgorithm( new QgsReverseLineDirectionAlgorithm() );
  addAlgorithm( new QgsRotateFeaturesAlgorithm() );
  addAlgorithm( new QgsRuggednessAlgorithm() );
  addAlgorithm( new QgsSaveSelectedFeatures() );
  addAlgorithm( new QgsSegmentizeByMaximumAngleAlgorithm() );
  addAlgorithm( new QgsSegmentizeByMaximumDistanceAlgorithm() );
  addAlgorithm( new QgsSelectByLocationAlgorithm() );
  addAlgorithm( new QgsServiceAreaFromLayerAlgorithm() );
  addAlgorithm( new QgsServiceAreaFromPointAlgorithm() );
  addAlgorithm( new QgsSetMValueAlgorithm() );
  addAlgorithm( new QgsSetZValueAlgorithm() );
  addAlgorithm( new QgsShortestPathLayerToPointAlgorithm() );
  addAlgorithm( new QgsShortestPathPointToLayerAlgorithm() );
  addAlgorithm( new QgsShortestPathPointToPointAlgorithm() );
  addAlgorithm( new QgsSimplifyAlgorithm() );
  addAlgorithm( new QgsSingleSidedBufferAlgorithm() );
  addAlgorithm( new QgsSlopeAlgorithm() );
  addAlgorithm( new QgsSmoothAlgorithm() );
  addAlgorithm( new QgsSnapToGridAlgorithm() );
  addAlgorithm( new QgsSpatialIndexAlgorithm() );
  addAlgorithm( new QgsSplitFeaturesByAttributeCharacterAlgorithm() );
  addAlgorithm( new QgsSplitGeometryAtAntimeridianAlgorithm() );
  addAlgorithm( new QgsSplitLinesByLengthAlgorithm() );
  addAlgorithm( new QgsSplitWithLinesAlgorithm() );
  addAlgorithm( new QgsStringConcatenationAlgorithm() );
  addAlgorithm( new QgsStyleFromProjectAlgorithm() );
  addAlgorithm( new QgsSubdivideAlgorithm() );
  addAlgorithm( new QgsSwapXYAlgorithm() );
  addAlgorithm( new QgsSymmetricalDifferenceAlgorithm() );
  addAlgorithm( new QgsTaperedBufferAlgorithm() );
  addAlgorithm( new QgsTransectAlgorithm() );
  addAlgorithm( new QgsTransformAlgorithm() );
  addAlgorithm( new QgsTranslateAlgorithm() );
  addAlgorithm( new QgsUnionAlgorithm() );
  addAlgorithm( new QgsVariableWidthBufferByMAlgorithm() );
  addAlgorithm( new QgsWedgeBuffersAlgorithm() );
  addAlgorithm( new QgsZonalHistogramAlgorithm() );
  addAlgorithm( new QgsPolygonsToLinesAlgorithm() );
  addAlgorithm( new QgsDensifyGeometriesByIntervalAlgorithm() );
  addAlgorithm( new QgsDensifyGeometriesByCountAlgorithm() );
}

///@endcond
