/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>
    SPDX-FileCopyrightText: 2018 Alex Nemeth <alex.nemeth329@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BLUR_H
#define BLUR_H

#include <kwineffects.h>
#include <kwinglplatform.h>
#include <kwinglutils.h>

#include <QVector>
#include <QVector2D>
#include <QStack>

#include <KWaylandServer/blur_interface.h>
#include <KWaylandServer/utils.h>

namespace KWin
{

static const int borderSize = 5;

class BlurShader;

class BlurEffect : public KWin::Effect
{
    Q_OBJECT

public:
    BlurEffect();
    ~BlurEffect() override;

    static bool supported();
    static bool enabledByDefault();

    void reconfigure(ReconfigureFlags flags) override;
    void prePaintScreen(ScreenPrePaintData &data, std::chrono::milliseconds presentTime) override;
    void prePaintWindow(EffectWindow* w, WindowPrePaintData& data, std::chrono::milliseconds presentTime) override;
    void drawWindow(EffectWindow *w, int mask, const QRegion &region, WindowPaintData &data) override;
    void paintEffectFrame(EffectFrame *frame, const QRegion &region, double opacity, double frameOpacity) override;

    bool provides(Feature feature) override;
    bool isActive() const override;

    int requestedEffectChainPosition() const override {
        return 75;
    }

    bool eventFilter(QObject *watched, QEvent *event) override;

    bool blocksDirectScanout() const override;

public Q_SLOTS:
    void slotWindowAdded(KWin::EffectWindow *w);
    void slotWindowDeleted(KWin::EffectWindow *w);
    void slotPropertyNotify(KWin::EffectWindow *w, long atom);
    void slotScreenGeometryChanged();

private:
    QRect expand(const QRect &rect) const;
    QRegion expand(const QRegion &region) const;
    bool renderTargetsValid() const;
    void deleteFBOs();
    void initBlurStrengthValues();
    void updateTexture();
    QRegion blurRegion(const EffectWindow *w) const;
    bool shouldBlur(const EffectWindow *w, int mask, const WindowPaintData &data) const;
    void updateBlurRegion(EffectWindow *w) const;
    void doBlur(const QRegion &shape, const QRect &screen, const float opacity, const QMatrix4x4 &screenProjection, bool isDock, QRect windowRect);
    void uploadRegion(QVector2D *&map, const QRegion &region, const int downSampleIterations);
    void uploadGeometry(GLVertexBuffer *vbo, const QRegion &blurRegion, const QRegion &windowRegion);
    void generateNoiseTexture();

    void upscaleRenderToScreen(GLVertexBuffer *vbo, int vboStart, int blurRectCount, const QMatrix4x4 &screenProjection, QPoint windowPosition);
    void applyNoise(GLVertexBuffer *vbo, int vboStart, int blurRectCount, const QMatrix4x4 &screenProjection, QPoint windowPosition);
    void downSampleTexture(GLVertexBuffer *vbo, int blurRectCount);
    void upSampleTexture(GLVertexBuffer *vbo, int blurRectCount);
    void copyScreenSampleTexture(GLVertexBuffer *vbo, int blurRectCount, QRegion blurShape, const QMatrix4x4 &screenProjection);

    void updateCornersRegion();
    
private:
    BlurShader *m_shader;
    QVector <GLRenderTarget*> m_renderTargets;
    QVector <GLTexture> m_renderTextures;
    QStack <GLRenderTarget*> m_renderTargetStack;

    QScopedPointer<GLTexture> m_noiseTexture;

    bool m_renderTargetsValid;
    long net_wm_blur_region;
    QRegion m_paintedArea; // keeps track of all painted areas (from bottom to top)
    QRegion m_currentBlur; // keeps track of the currently blured area of the windows(from bottom to top)

    int m_topRadius; // Radius of the rounded corners of the decoration
    int m_bottomRadius;
    QRegion m_topLeftCorner; // Regions we have to remove of the blured region
    QRegion m_topRightCorner;
    QRegion m_bottomLeftCorner;
    QRegion m_bottomRightCorner;
    
    int m_downSampleIterations; // number of times the texture will be downsized to half size
    int m_offset;
    int m_expandSize;
    int m_noiseStrength;
    int m_scalingFactor;
    bool m_roundedBottom;


    struct OffsetStruct {
        float minOffset;
        float maxOffset;
        int expandSize;
    };

    QVector <OffsetStruct> blurOffsets;

    struct BlurValuesStruct {
        int iteration;
        float offset;
    };

    QVector <BlurValuesStruct> blurStrengthValues;

    QMap <EffectWindow*, QMetaObject::Connection> windowBlurChangedConnections;
    KWaylandServer::ScopedGlobalPointer<KWaylandServer::BlurManagerInterface> m_blurManager;
};

inline
bool BlurEffect::provides(Effect::Feature feature)
{
    if (feature == Blur) {
        return true;
    }
    return KWin::Effect::provides(feature);
}


} // namespace KWin

#endif

