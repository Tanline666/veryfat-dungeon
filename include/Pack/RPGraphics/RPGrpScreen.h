#ifndef RP_GRAPHICS_SCREEN_H
#define RP_GRAPHICS_SCREEN_H
#include <Pack/types_pack.h>

#include <egg/gfxe.h>

//! @addtogroup rp_graphics
//! @{

// Forward declarations
class RPGrpTexture;

/**
 * \~english @brief Screen rendering surface
 */
class RPGrpScreen : public EGG::Screen {
public:
    /**
     * \~english @brief Initializes the screen system
     *
     * \~english @param maxX Screen width limits for all TV modes
     * \~english @param maxY Screen height limits for all TV modes
     */
    static void Initialize(const u16 maxX[TV_MODE_MAX],
                           const u16 maxY[TV_MODE_MAX]);

    /**
     * \~english @brief Constructor
     */
    RPGrpScreen() {
        SetFarZ(100000.0f);
    }

    /**
     * \~english @brief Constructor
     *
     * \~english @param x X position
     * \~english @param y Y position
     * \~english @param width Screen width
     * \~english @param height Screen height
     * \~english @param pParent Parent screen
     * \~english @param canvasMode Coordinate system
     */
    RPGrpScreen(f32 x, f32 y, f32 width, f32 height,
                const EGG::Screen* pParent = NULL,
                CanvasMode canvasMode = CANVAS_LU)
        : EGG::Screen(x, y, width, height, pParent, canvasMode) {}

    /**
     * \~english @brief Configures the GX rendering state to draw a quad
     */
    void BeginDrawQuad() const;

    /**
     * \~english @brief Draws a colored quad to this screen
     *
     * \~english @param x1 Left point of the quad
     * \~english @param y1 Top point of the quad
     * \~english @param x2 Right point of the quad
     * \~english @param y2 Bottom point of the quad
     * \~english @param color Fill color
     */
    void DrawQuad(f32 x1, f32 y1, f32 x2, f32 y2, GXColor color) const;

    /**
     * \~english @brief Configures the GX rendering state to draw a textured
     * quad
     *
     * \~english @param alpha Whether to enable the raster alpha channel
     */
    void BeginDrawQuadTexture(bool alpha) const;

    /**
     * \~english @brief Draws a textured quad to this screen
     * @details It is assumed the texture has already been loaded to GX_TEXMAP0.
     *
     * \~english @param x1 Left point of the quad
     * \~english @param y1 Top point of the quad
     * \~english @param x2 Right point of the quad
     * \~english @param y2 Bottom point of the quad
     * \~english @param color Material color
     */
    void DrawQuadTexture(f32 x1, f32 y1, f32 x2, f32 y2, GXColor color) const;

    /**
     * \~english @brief Draws a textured quad to this screen
     *
     * \~english @param x1 Left point of the quad
     * \~english @param y1 Top point of the quad
     * \~english @param x2 Right point of the quad
     * \~english @param y2 Bottom point of the quad
     * \~english @param rTexture Fill texture
     * \~english @param color Material color
     */
    void DrawQuadTexture(f32 x1, f32 y1, f32 x2, f32 y2,
                         const RPGrpTexture& rTexture, GXColor color) const;

    /**
     * \~english @brief Draws a textured quad to this screen
     *
     * \~english @param x1 Left point of the quad
     * \~english @param y1 Top point of the quad
     * \~english @param x2 Right point of the quad
     * \~english @param y2 Bottom point of the quad
     * \~english @param pResTIMG Fill texture
     * \~english @param color Material color
     */
    void DrawQuadTexture(f32 x1, f32 y1, f32 x2, f32 y2,
                         const EGG::ResTIMG* pResTIMG, GXColor color) const;

    /**
     * \~english @brief Accesses the root of the screen hierarchy
     */
    static RPGrpScreen& GetRootScreen() {
        return sRootScreen;
    }

private:
    //! Screen hierarchy root
    static RPGrpScreen sRootScreen;
};

//! @}

#endif
