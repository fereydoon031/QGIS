/***************************************************************************
    qgsmeshrenderervectorsettingswidget.cpp
    ---------------------------------------
    begin                : June 2018
    copyright            : (C) 2018 by Peter Petrik
    email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmeshrenderervectorsettingswidget.h"

#include "qgis.h"
#include "qgsmeshlayer.h"
#include "qgsmessagelog.h"

QgsMeshRendererVectorSettingsWidget::QgsMeshRendererVectorSettingsWidget( QWidget *parent )
  : QWidget( parent )

{
  setupUi( this );

  mShaftLengthComboBox->setCurrentIndex( -1 );

  connect( mColorWidget, &QgsColorButton::colorChanged, this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );
  connect( mLineWidthSpinBox, qgis::overload<double>::of( &QgsDoubleSpinBox::valueChanged ),
           this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );

  connect( mShaftLengthComboBox, qgis::overload<int>::of( &QComboBox::currentIndexChanged ),
           this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );

  connect( mShaftLengthComboBox, qgis::overload<int>::of( &QComboBox::currentIndexChanged ),
           mShaftOptionsStackedWidget, &QStackedWidget::setCurrentIndex );

  connect( mDisplayVectorsOnGridGroupBox, &QGroupBox::toggled, this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );

  QVector<QLineEdit *> widgets;
  widgets << mMinMagLineEdit << mMaxMagLineEdit
          << mHeadWidthLineEdit << mHeadLengthLineEdit
          << mMinimumShaftLineEdit << mMaximumShaftLineEdit
          << mScaleShaftByFactorOfLineEdit << mShaftLengthLineEdit;

  for ( auto widget : widgets )
  {
    connect( widget, &QLineEdit::textChanged, this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );
  }

  connect( mXSpacingSpinBox, qgis::overload<int>::of( &QgsSpinBox::valueChanged ), this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );
  connect( mYSpacingSpinBox, qgis::overload<int>::of( &QgsSpinBox::valueChanged ), this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );

  connect( mSymbologyVectorComboBox, qgis::overload<int>::of( &QComboBox::currentIndexChanged ),
           this, &QgsMeshRendererVectorSettingsWidget::onSymbologyChanged );
  onSymbologyChanged( 0 );

  connect( mSymbologyVectorComboBox, qgis::overload<int>::of( &QComboBox::currentIndexChanged ),
           this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );

  connect( mStreamlinesSeedingMethodComboBox, qgis::overload<int>::of( &QComboBox::currentIndexChanged ),
           this, &QgsMeshRendererVectorSettingsWidget::onStreamLineSeedingMethodChanged );
  onStreamLineSeedingMethodChanged( 0 );

  connect( mStreamlinesSeedingMethodComboBox, qgis::overload<int>::of( &QComboBox::currentIndexChanged ),
           this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );

  connect( mStreamlinesDensitySpinBox, qgis::overload<double>::of( &QgsDoubleSpinBox::valueChanged ),
           this, &QgsMeshRendererVectorSettingsWidget::widgetChanged );
}

void QgsMeshRendererVectorSettingsWidget::setLayer( QgsMeshLayer *layer )
{
  mMeshLayer = layer;
}

QgsMeshRendererVectorSettings QgsMeshRendererVectorSettingsWidget::settings() const
{
  QgsMeshRendererVectorSettings  settings;
  settings.setSymbology(
    static_cast<QgsMeshRendererVectorSettings::Symbology>( mSymbologyVectorComboBox->currentIndex() ) );

  //Arrow settings
  QgsMeshRendererVectorArrowSettings arrowSettings;

  // basic
  settings.setColor( mColorWidget->color() );
  settings.setLineWidth( mLineWidthSpinBox->value() );

  // filter by magnitude
  double val = filterValue( mMinMagLineEdit->text(), -1 );
  settings.setFilterMin( val );

  val = filterValue( mMaxMagLineEdit->text(), -1 );
  settings.setFilterMax( val );

  // arrow head
  val = filterValue( mHeadWidthLineEdit->text(), arrowSettings.arrowHeadWidthRatio() * 100.0 );
  arrowSettings.setArrowHeadWidthRatio( val / 100.0 );

  val = filterValue( mHeadLengthLineEdit->text(), arrowSettings.arrowHeadLengthRatio() * 100.0 );
  arrowSettings.setArrowHeadLengthRatio( val / 100.0 );

  // user grid
  bool enabled = mDisplayVectorsOnGridGroupBox->isChecked();
  settings.setOnUserDefinedGrid( enabled );
  settings.setUserGridCellWidth( mXSpacingSpinBox->value() );
  settings.setUserGridCellHeight( mYSpacingSpinBox->value() );

  // shaft length
  auto method = static_cast<QgsMeshRendererVectorArrowSettings::ArrowScalingMethod>( mShaftLengthComboBox->currentIndex() );
  arrowSettings.setShaftLengthMethod( method );

  val = filterValue( mMinimumShaftLineEdit->text(), arrowSettings.minShaftLength() );
  arrowSettings.setMinShaftLength( val );

  val = filterValue( mMaximumShaftLineEdit->text(), arrowSettings.maxShaftLength() );
  arrowSettings.setMaxShaftLength( val );

  val = filterValue( mScaleShaftByFactorOfLineEdit->text(), arrowSettings.scaleFactor() );
  arrowSettings.setScaleFactor( val );

  val = filterValue( mShaftLengthLineEdit->text(), arrowSettings.fixedShaftLength() );
  arrowSettings.setFixedShaftLength( val );

  settings.setArrowsSettings( arrowSettings );

  //Streamline setting
  QgsMeshRendererVectorStreamlineSettings streamlineSettings;
  streamlineSettings.setSeedingMethod(
    static_cast<QgsMeshRendererVectorStreamlineSettings::SeedingStartPointsMethod>( mStreamlinesSeedingMethodComboBox->currentIndex() ) );

  streamlineSettings.setSeedingDensity( mStreamlinesDensitySpinBox->value() / 100 );

  settings.setStreamLinesSettings( streamlineSettings );

  return settings;
}

void QgsMeshRendererVectorSettingsWidget::syncToLayer( )
{
  if ( !mMeshLayer )
    return;

  if ( mActiveDatasetGroup < 0 )
    return;

  const QgsMeshRendererSettings rendererSettings = mMeshLayer->rendererSettings();
  const QgsMeshRendererVectorSettings settings = rendererSettings.vectorSettings( mActiveDatasetGroup );

  mSymbologyVectorComboBox->setCurrentIndex( settings.symbology() );

  // Arrow settings
  const QgsMeshRendererVectorArrowSettings arrowSettings = settings.arrowSettings();

  // basic
  mColorWidget->setColor( settings.color() );
  mLineWidthSpinBox->setValue( settings.lineWidth() );

  // filter by magnitude
  if ( settings.filterMin() > 0 )
  {
    mMinMagLineEdit->setText( QString::number( settings.filterMin() ) );
  }
  if ( settings.filterMax() > 0 )
  {
    mMaxMagLineEdit->setText( QString::number( settings.filterMax() ) );
  }

  // arrow head
  mHeadWidthLineEdit->setText( QString::number( arrowSettings.arrowHeadWidthRatio() * 100.0 ) );
  mHeadLengthLineEdit->setText( QString::number( arrowSettings.arrowHeadLengthRatio() * 100.0 ) );

  // user grid
  mDisplayVectorsOnGridGroupBox->setChecked( settings.isOnUserDefinedGrid() );
  mXSpacingSpinBox->setValue( settings.userGridCellWidth() );
  mYSpacingSpinBox->setValue( settings.userGridCellHeight() );

  // shaft length
  mShaftLengthComboBox->setCurrentIndex( arrowSettings.shaftLengthMethod() );

  mMinimumShaftLineEdit->setText( QString::number( arrowSettings.minShaftLength() ) );
  mMaximumShaftLineEdit->setText( QString::number( arrowSettings.maxShaftLength() ) );
  mScaleShaftByFactorOfLineEdit->setText( QString::number( arrowSettings.scaleFactor() ) );
  mShaftLengthLineEdit->setText( QString::number( arrowSettings.fixedShaftLength() ) );

  //Streamlines settings
  const QgsMeshRendererVectorStreamlineSettings streamlinesSettings = settings.streamLinesSettings();;

  mStreamlinesSeedingMethodComboBox->setCurrentIndex( streamlinesSettings.seedingMethod() );
  mStreamlinesDensitySpinBox->setValue( streamlinesSettings.seedingDensity() * 100 );
}

void QgsMeshRendererVectorSettingsWidget::onSymbologyChanged( int currentIndex )
{
  mStreamlineWidget->setVisible( currentIndex == QgsMeshRendererVectorSettings::Streamlines );
  mArrowWidget->setVisible( currentIndex == QgsMeshRendererVectorSettings::Arrows );

  mDisplayVectorsOnGridGroupBox->setEnabled(
    currentIndex == QgsMeshRendererVectorSettings::Arrows ||
    ( currentIndex == QgsMeshRendererVectorSettings::Streamlines &&
      mStreamlinesSeedingMethodComboBox->currentIndex() == QgsMeshRendererVectorStreamlineSettings::MeshGridded ) );
}

void QgsMeshRendererVectorSettingsWidget::onStreamLineSeedingMethodChanged( int currentIndex )
{
  bool enabled = currentIndex == QgsMeshRendererVectorStreamlineSettings::Random;
  mStreamlinesDensityLabel->setEnabled( enabled );
  mStreamlinesDensitySpinBox->setEnabled( enabled );

  mDisplayVectorsOnGridGroupBox->setEnabled( !enabled );
}

double QgsMeshRendererVectorSettingsWidget::filterValue( const QString &text, double errVal ) const
{
  if ( text.isEmpty() )
    return errVal;

  bool ok;
  double val = text.toDouble( &ok );
  if ( !ok )
    return errVal;

  if ( val < 0 )
    return errVal;

  return val;
}
