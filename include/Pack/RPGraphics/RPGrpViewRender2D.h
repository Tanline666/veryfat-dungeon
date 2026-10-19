#ifndef RP_GRAPHICS_VIEW_RENDER_2D_H
#define RP_GRAPHICS_VIEW_RENDER_2D_H
#include <Pack/types_pack.h>

#include <Pack/RPGraphics/RPGrpView.h>

//! @addtogroup rp_graphics
//! @{

/**
 * \~english @brief Renderer view for 2D graphics
 */
class RPGrpViewRender2D : public RPGrpView {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param no View ID
     */
    explicit RPGrpViewRender2D(u8 no);

    /**
     * \~english @brief Gets the type of this view
     */
    virtual EType GetType() const override { // at 0xC
        return EType_Render2D;
    }

    /**
     * \~english @brief Initializes this view's state
     */
    virtual void Configure() override; // at 0x18

    /**
     * \~english @brief Prepares this view for a new frame
     */
    virtual void PrepareDraw() override; // at 0x20

    /**
     * \~english @brief Renders the contents of this view
     */
    virtual void Draw() override; // at 0x24
};

//! @}

#endif
