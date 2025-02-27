/***************************************************************************
                         qgsmeshtracerenderer.h
                         -------------------------
    begin                : November 2019
    copyright            : (C) 2019 by Vincent Cloarec
    email                : vcloarec at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSMESHTRACERENDERER_H
#define QGSMESHTRACERENDERER_H


#include <QVector>
#include <QSize>

#include "qgis_core.h"
#include "qgis.h"
#include "qgstriangularmesh.h"
#include "qgsmeshlayer.h"
#include "qgsmeshlayerutils.h"
#include "qgsmeshvectorrenderer.h"


///@cond PRIVATE

#ifndef SIP_RUN

/**
 * \ingroup core
 *
 * Abstract class used to interpolate the value of the vector for a pixel
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
class QgsMeshVectorValueInterpolator
{
  public:
    //! Constructor
    QgsMeshVectorValueInterpolator( const QgsTriangularMesh &triangularMesh,
                                    const QgsMeshDataBlock &datasetVectorValues );

    //! Constructor with scalar active face flag values to not interpolate on inactive face
    QgsMeshVectorValueInterpolator( const QgsTriangularMesh &triangularMesh,
                                    const QgsMeshDataBlock &datasetVectorValues,
                                    const QgsMeshDataBlock &scalarActiveFaceFlagValues );

    //! Copy constructor
    QgsMeshVectorValueInterpolator( const QgsMeshVectorValueInterpolator &other );

    //! Clone
    virtual QgsMeshVectorValueInterpolator *clone() = 0;

    //! Destructor
    virtual ~QgsMeshVectorValueInterpolator() = default;

    //! Returns the interpolated vector
    virtual QgsVector vectorValue( const QgsPointXY &point ) const;

    //! Assignment operator
    QgsMeshVectorValueInterpolator &operator=( const QgsMeshVectorValueInterpolator &other );

  protected:
    void updateCacheFaceIndex( const QgsPointXY &point ) const;

    QgsTriangularMesh mTriangularMesh;
    QgsMeshDataBlock mDatasetValues;
    QgsMeshDataBlock mActiveFaceFlagValues;
    mutable QgsMeshFace mFaceCache;
    mutable int mCacheFaceIndex = -1;
    bool mUseScalarActiveFaceFlagValues = false;
    bool isVectorValid( const QgsVector &v ) const;

  private:

    void activeFaceFilter( QgsVector &vector, int faceIndex ) const;

    virtual QgsVector interpolatedValuePrivate( int faceIndex, const QgsPointXY point ) const = 0;
};

/**
 * \ingroup core
 *
 * Class used to retrieve the value of the vector for a pixel from vertex
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
class QgsMeshVectorValueInterpolatorFromVertex: public QgsMeshVectorValueInterpolator
{
  public:
    //! Constructor
    QgsMeshVectorValueInterpolatorFromVertex( const QgsTriangularMesh &triangularMesh,
        const QgsMeshDataBlock &datasetVectorValues );

    //! Constructor with scalar active face flag value to not interpolate on inactive face
    QgsMeshVectorValueInterpolatorFromVertex( const QgsTriangularMesh &triangularMesh,
        const QgsMeshDataBlock &datasetVectorValues,
        const QgsMeshDataBlock &scalarActiveFaceFlagValues );

    //! Copy constructor
    QgsMeshVectorValueInterpolatorFromVertex( const QgsMeshVectorValueInterpolatorFromVertex &other );

    //! Clone the instance
    virtual QgsMeshVectorValueInterpolatorFromVertex *clone() override;

    //! Assignment operator
    QgsMeshVectorValueInterpolatorFromVertex &operator=( const QgsMeshVectorValueInterpolatorFromVertex &other );

  private:
    QgsVector interpolatedValuePrivate( int faceIndex, const QgsPointXY point ) const override;
};

/**
 * \ingroup core
 *
 * Class used to retrieve the value of the vector for a pixel from vertex
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
class QgsMeshVectorValueInterpolatorFromFace: public QgsMeshVectorValueInterpolator
{
  public:
    //! Constructor
    QgsMeshVectorValueInterpolatorFromFace( const QgsTriangularMesh &triangularMesh,
                                            const QgsMeshDataBlock &datasetVectorValues );

    //! Constructor with scalar active face flag value to not interpolate on inactive face
    QgsMeshVectorValueInterpolatorFromFace( const QgsTriangularMesh &triangularMesh,
                                            const QgsMeshDataBlock &datasetVectorValues,
                                            const QgsMeshDataBlock &scalarActiveFaceFlagValues );

    //! Copy constructor
    QgsMeshVectorValueInterpolatorFromFace( const QgsMeshVectorValueInterpolatorFromFace &other );

    //! Clone the instance
    virtual QgsMeshVectorValueInterpolatorFromFace *clone() override;

    //! Assignment operator
    QgsMeshVectorValueInterpolatorFromFace &operator=( const QgsMeshVectorValueInterpolatorFromFace &other );

  private:
    QgsVector interpolatedValuePrivate( int faceIndex, const QgsPointXY point ) const override;
};

/**
 * \ingroup core
 *
 * Abstract class used to handle information about stream field
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
class QgsMeshStreamField
{
  public:
    struct FieldData
    {
      double magnitude;
      float time;
      int directionX;
      int directionY;
    };

    //! Constructor
    QgsMeshStreamField( const QgsTriangularMesh &triangularMesh,
                        const QgsMeshDataBlock &dataSetVectorValues,
                        const QgsMeshDataBlock &scalarActiveFaceFlagValues,
                        const QgsRectangle &layerExtent,
                        double magnitudeMaximum, bool dataIsOnVertices, const QgsRenderContext &rendererContext,
                        int resolution = 1 );

    //! Copy constructor
    QgsMeshStreamField( const QgsMeshStreamField &other );

    //! Destructor
    virtual ~QgsMeshStreamField();

    /**
    * Updates the size of the field and the QgsMapToPixel instance to retrieve map point
    * from pixel in the field depending on the resolution of the device
    * If the extent of renderer context and the resolution are not changed, do nothing
    * else, updates the size and cleans
    */
    void updateSize( const QgsRenderContext &renderContext );

    /**
    * Updates the size of the field and the QgsMapToPixel instance to retrieve map point
    * from pixel in the field depending on the resolution of the device
    */
    void updateSize( const QgsRenderContext &renderContext, int resolution );

    //! Returns true if the field is valid
    bool isValid() const;

    //! Returns the size of the field
    QSize size() const;

    //! Returns the topLeft of the field in the device coordinate
    QPoint topLeft() const;

    //! Adds a trace in the field from a start pixel
    void addTrace( QPoint startPixel );

    //! Adds a trace in the field from a map point
    void addTrace( QgsPointXY startPoint );

    //! Adds random traces in the field from random start points, the number of traces depends on the max filling density
    void addRandomTraces();

    //! Adds a trace in the field from one random start point
    void addRandomTrace();

    //! Adds traces in the field from gridded start points, pixelSpace is the space between points in pixel field
    void addGriddedTraces( int dx, int dy );

    //! Adds traces in the field from vertex on a mesh
    void addTracesOnMesh( const QgsTriangularMesh &mesh, const QgsRectangle &extent );

    //! Sets the resolution of the field
    void setResolution( int width );

    //! Returns the width of particle
    int resolution() const;

    //! Returns the size of the image that represents the trace field
    QSize imageSize() const;

    //! Returns the current render image of the field
    virtual QImage image();

    //! Sets the maximum pixel filling, eg, the rate of number pixel that can be filled with way.
    void setPixelFillingDensity( double maxFilling );

    //! Sets  color of the streamlines
    void setColor( QColor color );

    //! Sets line width of the streamlines
    void setLineWidth( double width );

    //! Sets min/max filter
    void setFilter( double min, double max );

    //! Sets if the size of the field has to be minimized of all the mesh is in the device
    void setMinimizeFieldSize( bool minimizeFieldSize );

    //! Assignment operator
    QgsMeshStreamField &operator=( const QgsMeshStreamField &other );

  protected:
    void initImage();
    QPointF fieldToDevice( const QPoint &pixel ) const;
    bool filterMag( double value ) const;

  private:
    QgsPointXY positionToMapCoordinates( const QPoint &pixelPosition, const QgsPointXY &positionInPixel );
    bool addPixelToChunkTrace( QPoint &pixel,
                               QgsMeshStreamField::FieldData &data,
                               std::list<QPair<QPoint, QgsMeshStreamField::FieldData> > &chunkTrace );
    void setChunkTrace( std::list<QPair<QPoint, FieldData>> &chunkTrace );
    virtual void drawChunkTrace( const std::list<QPair<QPoint, FieldData>> &chunkTrace ) = 0;
    void clearChunkTrace( std::list<QPair<QPoint, FieldData>> &chunkTrace );
    virtual void storeInField( const QPair<QPoint, FieldData> pixelData ) = 0;
    virtual void initField() = 0;
    void simplifyChunkTrace( std::list<QPair<QPoint, FieldData>> &shunkTrace );

    virtual bool isTraceExists( const QPoint &pixel ) const = 0;
    bool isTraceOutside( const QPoint &pixel ) const;

  protected:

    QSize mFieldSize;
    QPainter *mPainter = nullptr;
    int mFieldResolution = 1;
    QPen mPen;
    QImage mTraceImage;

    QgsMapToPixel mMapToFieldPixel;

  private:
    int mPixelFillingCount = 0;
    int mMaxPixelFillingCount = 0;
    std::unique_ptr<QgsMeshVectorValueInterpolator> mVectorValueInterpolator;
    QgsRectangle mLayerExtent;
    QgsRectangle mMapExtent;
    QPoint mFieldTopLeftInDeviceCoordinates;
    bool mValid = false;
    double mMagMax = 0;
    double mPixelFillingDensity;
    double mMinMagFilter = -1;
    double mMaxMagFilter = -1;
    const QgsRenderContext &mRenderContext; //keep the renderer context only to know if the renderer is stopped
    bool mMinimizeFieldSize = true; //
};

/**
 * \ingroup core
 *
 * Class used to draw streamlines from vector field
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
class QgsMeshStreamlinesField: public QgsMeshStreamField
{
  public:
    //! Constructor
    QgsMeshStreamlinesField( const QgsTriangularMesh &triangularMesh,
                             const QgsMeshDataBlock &datasetVectorValues,
                             const QgsMeshDataBlock &scalarActiveFaceFlagValues,
                             const QgsRectangle &layerExtent,
                             double magMax, bool dataIsOnVertices, QgsRenderContext &rendererContext );

    //! Copy constructor
    QgsMeshStreamlinesField( const QgsMeshStreamlinesField &other );

    //! Assignment operator
    QgsMeshStreamlinesField &operator=( const QgsMeshStreamlinesField &other );

  private:
    void storeInField( const QPair<QPoint, FieldData> pixelData ) override;
    void initField() override;
    bool isTraceExists( const QPoint &pixel ) const override;
    void drawChunkTrace( const std::list<QPair<QPoint, FieldData> > &chunkTrace ) override;

    QVector<bool> mField;

};

class QgsMeshParticleTracesField;

/**
 * \ingroup core
 *
 * Used to simulation moving particle
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
struct QgsMeshTraceParticle
{
  double lifeTime = 0;
  QPoint position;
  std::list<QPoint> tail;
  double remainingTime = 0; //time remaining to spend in the current pixel at the end of the time step
};

/**
 * \ingroup core
 *
 * Class used to draw streamlines from vector field
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
class QgsMeshParticleTracesField: public QgsMeshStreamField
{
  public:
    //! Constructor
    QgsMeshParticleTracesField( const QgsTriangularMesh &triangularMesh,
                                const QgsMeshDataBlock &datasetVectorValues,
                                const QgsMeshDataBlock &scalarActiveFaceFlagValues,
                                const QgsRectangle &layerExtent,
                                double magMax,
                                bool dataIsOnVertices,
                                const QgsRenderContext &rendererContext );

    //! Copy constructor
    QgsMeshParticleTracesField( const QgsMeshParticleTracesField &other );

    //! Adds a particle in the vector field from a start point (pixel) with a specified life time
    void addParticle( const QPoint &startPoint, double lifeTime );

    //! Adds a particle in the vector field from a start point (map point) with a specified life time
    void addParticleXY( const QgsPointXY &startPoint, double lifeTime );

    //! Adds particle randomly (position and life time
    void addRandomParticles();

    //! Moves all the particles with a displacement corresponding to a nondimensional time
    void moveParticles();

    //! Returns the current image of the particles
    QImage imageRendered() const;

    //! Sets the total number of particles generated randomly
    void setParticlesCount( int particlesCount );

    //! Sets the maximum life time (nondimensional) of particle generated
    void setParticlesLifeTime( double particlesLifeTime );

    //! Stumps particles image and leave a persistent effect
    void stump();

    /**
     * Sets stump factor from 0 to 255 :
     * 0, stump completely, no persistence
     * 255, no stump,  total persistence
     */
    void setStumpFactor( int sf );

    //! Sets the time step
    void setTimeStep( double timeStep );

    //! Sets tihe color of the particles
    void setParticleColor( const QColor &particleColor );

    //! Sets particles size
    void setParticleSize( double particleSize );

    //! Sets the tail factor
    void setTailFactor( double tailFactor );

    //! Sets the minimum tail length
    void setMinTailLength( int minTailLength );

    //! Assignment operator
    QgsMeshParticleTracesField &operator=( const QgsMeshParticleTracesField &other );

  private:
    QPoint direction( QPoint position ) const;

    float time( QPoint position ) const;

    void drawParticleTrace( const QgsMeshTraceParticle &particle );

    void storeInField( const QPair<QPoint, FieldData> pixelData ) override;
    void initField() override;
    bool isTraceExists( const QPoint &pixel ) const override;
    void drawChunkTrace( const std::list<QPair<QPoint, FieldData>> &chunkTrace ) override {Q_UNUSED( chunkTrace );}

    /* Nondimensional time
     * This field store the time spent by the particle in the pixel
     *
     * This time is nondimensional and value 1 is equivalent to the time spent by the particle in a pixel
     * for Vmax, the maximum magnitude of the vector field.
     *
     */
    QVector<float> mTimeField;

    /*the direction for a pixel is defined with a char value
     *
     *     1  2  3
     *     4  5  6
     *     7  8  9
     *
     *     convenient to retrieve the indexes of the next pixel from the direction d:
     *     Xnext= (d-1)%3-1
     *     Ynext = (d-1)/3-1
     *
     *     and the direction is defined by :
     *     d=incX + 2 + (incY+1)*3
     */
    QVector<char> mDirectionField;
    QList<QgsMeshTraceParticle> mParticles;
    QImage mStumpImage;

    double mTimeStep = 200;
    double mParticlesLifeTime = 5000;
    int mParticlesCount = 1000;
    double mTailFactor = 5;
    int mMinTailLength = 3;
    QColor mParticleColor = Qt::white;
    double mParticleSize = 2.5;
    int mStumpFactor = 50;
};

/**
 * \ingroup core
 *
 * A class derived from QgsMeshVectorRenderer used to render the particles traces
 *
 * \note not available in Python bindings
 * \since QGIS 3.12
 */
class QgsMeshVectorStreamlineRenderer: public QgsMeshVectorRenderer
{
  public:
    //!Constructor
    QgsMeshVectorStreamlineRenderer( const QgsTriangularMesh &triangularMesh,
                                     const QgsMeshDataBlock &dataSetVectorValues,
                                     const QgsMeshDataBlock &scalarActiveFaceFlagValues,
                                     bool dataIsOnVertices,
                                     const QgsMeshRendererVectorSettings &settings,
                                     QgsRenderContext &rendererContext,
                                     const QgsRectangle &layerExtent,
                                     double magMax );

    void draw() override;

  private:
    std::unique_ptr<QgsMeshStreamField> mStreamLineField;
    QgsRenderContext &mRendererContext;
};

#endif //SIP_RUN

///@endcond

/**
 * \ingroup core
 *
 * A wrapper for QgsMeshParticuleTracesField used to render the particles. Available for Python binding
 *
 * \since QGIS 3.12
 */
class CORE_EXPORT QgsMeshVectorTraceRenderer
{
  public:
    //!Constructor to use from QgsMeshVectorRenderer
    QgsMeshVectorTraceRenderer( const QgsTriangularMesh &triangularMesh,
                                const QgsMeshDataBlock &dataSetVectorValues,
                                const QgsMeshDataBlock &scalarActiveFaceFlagValues,
                                bool dataIsOnVertices,
                                const QgsRenderContext &rendererContext,
                                const QgsRectangle &layerExtent,
                                double magMax ) SIP_SKIP;

    //!Constructor to use with Python binding
    QgsMeshVectorTraceRenderer( QgsMeshLayer *layer, const QgsRenderContext &rendererContext );

    //! Copy constructor
    QgsMeshVectorTraceRenderer( const QgsMeshVectorTraceRenderer &other );

    //! Destructor
    ~QgsMeshVectorTraceRenderer();

    //! seeds particles in the vector fields
    void seedRandomParticles( int count );

    //! Moves all the particles using frame per second (fps) to calculate the displacement
    QImage imageRendered();

    //! Sets the number of frames per seconds that will be rendered
    void setFPS( int FPS );

    //! Sets the max number of pixels that can be go through by the particles in 1 second
    void setMaxSpeedPixel( int max );

    //! Sets maximum life time of particles in seconds
    void setParticlesLifeTime( double particleLifeTime );

    //! Sets colors of particle
    void setParticlesColor( const QColor &c );

    //! Sets particle size
    void setParticlesSize( double width );

    //! Sets the tail factor, used to adjust the length of the tail. 0 : minimum length, >1 increase the tail
    void setTailFactor( double fct );

    //! Sets the minimum tail length
    void setMinimumTailLength( int l );

    //! Sets the visual persistence of the tail
    void setTailPersitence( double p );

    //! Assignment operator
    QgsMeshVectorTraceRenderer &operator=( const QgsMeshVectorTraceRenderer &other );
  private:
    std::unique_ptr<QgsMeshParticleTracesField> mParticleField;
    const QgsRenderContext &mRendererContext;
    int mFPS = 15; //frame per second of the output, used to calculate orher parameters of the field
    int mVpixMax = 2000; //is the number of pixels that are going through for 1 s
    double mParticleLifeTime = 5;

    void updateFieldParameter();
};

#endif // QGSMESHTRACERENDERER_H
