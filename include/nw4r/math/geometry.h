#ifndef NW4R_MATH_GEOMETRY_H
#define NW4R_MATH_GEOMETRY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/types.h>

namespace nw4r {
namespace math {

struct AABB;
struct LINE3;
struct SEGMENT3;

/**
 * @~english @brief Enumerator describing the ways two given entities can
 * intersect.
 * @~japanese @brief 2つの指定された実体の交差方法を定義する列挙型。
 */
enum IntersectionResult {
    INTERSECTION_NONE,
    INTERSECTION_1,
    INTERSECTION_2,

    INTERSECTION_LINE3_ON_PLANE = INTERSECTION_2,
    INTERSECTION_RAY3_ON_PLANE = INTERSECTION_2,
    INTERSECTION_SEGMENT3_ON_PLANE = INTERSECTION_2,

    INTERSECTION_OUTSIDE = 0,
    INTERSECTION_INSIDE,
    INTERSECTION_INTERSECT
};

/**
 * @~english @brief Checks whether the two AABBs intersect.
 * @~japanese @brief 2つのAABBが交差するかどうかを調べる。
 *
 * @~english @param a Pointer to constant AABB, left-side
 * @~japanese @param a 左辺の定数のAABBへのポインタ
 * @~english @param b Pointer to constant AABB, right-side
 * @~japanese @param b 右辺の定数のAABBへのポインタ
 * @~english @return TRUE if intersecting
 * @~japanese @return 交差すればTRUE
 */
bool IntersectionAABB(const AABB* a, const AABB* b);

struct LINE3 {
public:
    LINE3() {}

    void Set(const SEGMENT3* S);
    void Normalize() {
        VEC3Normalize(&d, &d);
    }

public:
    VEC3 P;
    VEC3 d;
};

struct SEGMENT3 {
public:
    SEGMENT3() {}
    SEGMENT3(const f32* p) : P0(p), P1(p + 3) {}

public:
    VEC3 P0;
    VEC3 P1;
};

inline void LINE3::Set(const SEGMENT3* S) {
    P = S->P0;
    VEC3Sub(&d, &S->P1, &S->P0);
    Normalize();
}

f32 DistSqPoint3ToLine3(const VEC3* P, const LINE3* L, f32* t);
/**
 * @~english @brief Calculates the square of the distance between a point in 3D
 * space and a line segment.
 * @~japanese @brief 3次元空間での点と線分の距離の2乗を計算する。
 *
 * @~english @param P Pointer to vector representing a point in 3D space
 * @~japanese @param P 3次元空間での点を表現するベクトルへのポインタ
 * @~english @param S Pointer to SEGMENT3 object
 * @~japanese @param S SEGMENT3オブジェクトへのポインタ
 * @param t
 * @return f32
 */
f32 DistSqPoint3ToSegment3(const VEC3* P, const SEGMENT3* S, f32* t);

/**
 * @~english @brief Struct representing a sphere.
 * @~japanese @brief 球を表現するストラクト。
 */
struct SPHERE {
    /**
     * @~english @brief Constructor
     * @~japanese @brief コンストラクタ
     */
    SPHERE() {}

    VEC3 C; //!< @~english Center of the sphere @~japanese 球の中心 @~ // at 0x0
    f32 r;  //!< @~english Sphere radius @~japanese 球の半径 @~ // at 0xC
};

/**
 * @~english @brief Struct that represents a 3D plane.
 * @~japanese @brief 3D平面を表現するストラクト。
 */
struct PLANE {
    /**
     * @~english @brief Constructor
     * @~japanese @brief コンストラクタ
     */
    PLANE() {}

    /**
     * @~english @brief Calculates the distance from the given point relative to
     * the plane.
     * @~japanese @brief 指定された点と平面の距離を計算する。
     *
     * @~english @param P Constant reference to a VEC3 point
     * @~japanese @param P VEC3点への定数の参照
     * @~english @return The distance: if positive, it is in the direction of
     * the normal vector and vice versa
     * @~japanese @return
     * 距離：正数であれば点は法線ベクトルに向かって、逆もしかり
     */
    f32 Test(const VEC3& P) const {
        return d + VEC3Dot(&n, &P);
    }

    void Set(const VEC3* p0, const VEC3* p1, const VEC3* p2);

    VEC3 n; //!< @~english The plane's normal vector @~japanese
            //!< 平面の法線ベクトル @~ // at 0x0
    f32 d; //!< @~english Sign-reversed inner product of the normal vector and a
           //!< point on the plane @~japanese
           //!< 法線ベクトルと平面上の点の、符号を反転させた内積 @~ // at 0xC
};

/**
 * @~english @brief Struct that stores an axis-aligned bounding box (AABB).
 * @~japanese @brief 軸平行境界ボックス（Axis-Aligned Bounding Box,
 * AABB)を格納するストラクト。
 */
struct AABB {
    /**
     * @~english @brief Constructor
     * @~japanese @brief コンストラクタ
     */
    AABB() {}

    void Set(const VEC3* pPoints, unsigned int num);
    void Set(const AABB* pBox, const MTX34* pMtx);

    VEC3 min; //!< @~english Coordinate of the smallest corner of the box
              //!< @~japanese ボックスの最小の隅の座軸 @~ // at 0x0
    VEC3 max; //!< @~english Coordinate of the largest corner of the box
              //!< @~japanese ボックスの最大の隅の座軸 @~ // at 0xC
};

/**
 * @~english @brief Class that controls culling operations for the viewing
 * frustum.
 * @~japanese @brief ビューイングフラスタムのカリング処理を管理するクラス。
 */
class FRUSTUM {
private:
    enum Point {
        POINT_NEAR_TL,
        POINT_NEAR_TR,
        POINT_NEAR_BR,
        POINT_NEAR_BL,

        POINT_FAR_TL,
        POINT_FAR_TR,
        POINT_FAR_BR,
        POINT_FAR_BL,

        POINT_MAX
    };

    enum Plane {
        PLANE_L,
        PLANE_R,
        PLANE_N,
        PLANE_F,
        PLANE_T,
        PLANE_B,

        PLANE_MAX
    };

public:
    void Set(f32 fovy, f32 aspect, f32 n, f32 f, const MTX34& rCamMtx);
    void Set(f32 t, f32 b, f32 l, f32 r, f32 n, f32 f, const MTX34& rCamMtx);

    IntersectionResult IntersectAABB_Ex(const AABB* B) const;

private:
    MTX34 mCamMtx;            // at 0x0
    PLANE mPlaneL;            // at 0x30
    PLANE mPlaneR;            // at 0x40
    PLANE mPlaneT;            // at 0x50
    PLANE mPlaneB;            // at 0x60
    f32 mNearZ;               // at 0x70
    f32 mFarZ;                // at 0x74
    AABB mBox;                // at 0x78
    PLANE mPlanes[PLANE_MAX]; // at 0x90
};

} // namespace math
} // namespace nw4r

#endif
