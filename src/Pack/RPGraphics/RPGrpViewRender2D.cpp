#include <Pack/RPGraphics.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param no View ID
 */
RPGrpViewRender2D::RPGrpViewRender2D(u8 no) : RPGrpView(no) {
    mProjType = EProjType_Ortho;
}

/**
 * \~english @brief Initializes this view's state
 */
void RPGrpViewRender2D::Configure() {}

/**
 * \~english @brief Prepares this view for a new frame
 */
void RPGrpViewRender2D::PrepareDraw() {
    RPGrpView::PrepareDraw();
}

/**
 * \~english @brief Renders the contents of this view
 */
void RPGrpViewRender2D::Draw() {
    if (!IsSysCalc()) {
        return;
    }

    ClearBuffer();
    SetProjectionGX();
    EGG::StateGX::GXSetAlphaUpdate_(false);
}
