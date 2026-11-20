#ifndef NW4R_PUBLIC_EF_H
#define NW4R_PUBLIC_EF_H

#include <nw4r/ef/drawinfo.h>
#include <nw4r/ef/drawstrategy.h>
#include <nw4r/ef/effect.h>
#include <nw4r/ef/effectsystem.h>
#include <nw4r/ef/emitter.h>
#include <nw4r/ef/emitterform.h>
#include <nw4r/ef/field.h>
#include <nw4r/ef/handle.h>
#include <nw4r/ef/memorymanager.h>
#include <nw4r/ef/particle.h>
#include <nw4r/ef/particlemanager.h>
#include <nw4r/ef/postfield.h>
#include <nw4r/ef/res_animcurve.h>
#include <nw4r/ef/res_emitter.h>
#include <nw4r/ef/res_texture.h>
#include <nw4r/ef/resource.h>
#include <nw4r/ef/util.h>

/******************************************************************************
 *
 * Emitter forms
 *
 ******************************************************************************/

#include <nw4r/ef/emform/cube.h>
#include <nw4r/ef/emform/cylinder.h>
#include <nw4r/ef/emform/disc.h>
#include <nw4r/ef/emform/emform.h>
#include <nw4r/ef/emform/line.h>
#include <nw4r/ef/emform/point.h>
#include <nw4r/ef/emform/sphere.h>
#include <nw4r/ef/emform/torus.h>

/******************************************************************************
 *
 * Draw strategies
 *
 ******************************************************************************/

#include <nw4r/ef/drawstrategy/drawbillboardstrategy.h>
#include <nw4r/ef/drawstrategy/drawdirectionalstrategy.h>
#include <nw4r/ef/drawstrategy/drawfreestrategy.h>
#include <nw4r/ef/drawstrategy/drawlinestrategy.h>
#include <nw4r/ef/drawstrategy/drawpointstrategy.h>
#include <nw4r/ef/drawstrategy/drawsmoothstripestrategy.h>
#include <nw4r/ef/drawstrategy/drawstrategybuilder.h>
#include <nw4r/ef/drawstrategy/drawstrategyimpl.h>
#include <nw4r/ef/drawstrategy/drawstripestrategy.h>

#endif

/******************************************************************************
 *
 * Customizable memory manager
 *
 ******************************************************************************/
#if defined(NW4R_EF_MEMORY_MANAGER_CONFIGURED)

#ifndef NW4R_PUBLIC_EF_MEMORY_MANAGER_H
#define NW4R_PUBLIC_EF_MEMORY_MANAGER_H

#include <nw4r/ef/memorymanagerconfig.h>
#include <nw4r/ef/memorymanagerimpl.h>
#include <nw4r/ef/memorymanagertmp.h>

#endif

#endif
