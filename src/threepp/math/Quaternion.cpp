
#include "threepp/math/Quaternion.hpp"

#include "threepp/math/Matrix4.hpp"
#include "threepp/math/Vector3.hpp"
#include "threepp/math/Euler.hpp"

#include <cmath>
#include <algorithm>

using namespace threepp;

Quaternion::Quaternion(float x, float y, float z, float w) : x_(x), y_(y), z_(z), w_(w) {}

Quaternion &Quaternion::set(float x, float y, float z, float w) {

    this->x_ = x;
    this->y_ = y;
    this->z_ = z;
    this->w_ = w;

    this->onChangeCallback_();

    return *this;
}

Quaternion &Quaternion::setFromEuler(const Euler &euler, bool update) {

    const auto x = euler.x_, y = euler.y_, z = euler.z_;
    const auto order = euler.order_;

    // http://www.mathworks.com/matlabcentral/fileexchange/
    // 	20696-function-to-convert-between-dcm-euler-angles-quaternions-and-euler-vectors/
    //	content/SpinCalc.m

    const auto c1 = std::cos( x / 2 );
    const auto c2 = std::cos( y / 2 );
    const auto c3 = std::cos( z / 2 );

    const auto s1 = std::sin( x / 2 );
    const auto s2 = std::sin( y / 2 );
    const auto s3 = std::sin( z / 2 );

    switch ( order ) {

        case Euler::RotationOrders::XYZ:
            this->x_ = s1 * c2 * c3 + c1 * s2 * s3;
            this->y_ = c1 * s2 * c3 - s1 * c2 * s3;
            this->z_ = c1 * c2 * s3 + s1 * s2 * c3;
            this->w_ = c1 * c2 * c3 - s1 * s2 * s3;
            break;

        case Euler::RotationOrders::YXZ:
            this->x_ = s1 * c2 * c3 + c1 * s2 * s3;
            this->y_ = c1 * s2 * c3 - s1 * c2 * s3;
            this->z_ = c1 * c2 * s3 - s1 * s2 * c3;
            this->w_ = c1 * c2 * c3 + s1 * s2 * s3;
            break;

        case Euler::RotationOrders::ZXY:
            this->x_ = s1 * c2 * c3 - c1 * s2 * s3;
            this->y_ = c1 * s2 * c3 + s1 * c2 * s3;
            this->z_ = c1 * c2 * s3 + s1 * s2 * c3;
            this->w_ = c1 * c2 * c3 - s1 * s2 * s3;
            break;

        case Euler::RotationOrders::ZYX:
            this->x_ = s1 * c2 * c3 - c1 * s2 * s3;
            this->y_ = c1 * s2 * c3 + s1 * c2 * s3;
            this->z_ = c1 * c2 * s3 - s1 * s2 * c3;
            this->w_ = c1 * c2 * c3 + s1 * s2 * s3;
            break;

        case Euler::RotationOrders::YZX:
            this->x_ = s1 * c2 * c3 + c1 * s2 * s3;
            this->y_ = c1 * s2 * c3 + s1 * c2 * s3;
            this->z_ = c1 * c2 * s3 - s1 * s2 * c3;
            this->w_ = c1 * c2 * c3 - s1 * s2 * s3;
            break;

        case Euler::RotationOrders::XZY:
            this->x_ = s1 * c2 * c3 - c1 * s2 * s3;
            this->y_ = c1 * s2 * c3 - s1 * c2 * s3;
            this->z_ = c1 * c2 * s3 + s1 * s2 * c3;
            this->w_ = c1 * c2 * c3 + s1 * s2 * s3;
            break;

    }

    if ( !update ) this->onChangeCallback_();

    return *this;

}

Quaternion &Quaternion::setFromAxisAngle(const Vector3 &axis, float angle) {

    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm

    // assumes axis is normalized

    const auto halfAngle = angle / 2, s = std::sin(halfAngle);

    this->x_ = axis.x * s;
    this->y_ = axis.y * s;
    this->z_ = axis.z * s;
    this->w_ = std::cos(halfAngle);

    this->onChangeCallback_();

    return *this;
}

Quaternion &Quaternion::setFromRotationMatrix(const Matrix4 &m) {

    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm

    // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)

    const auto te = m.elements_;

    const auto m11 = te[0], m12 = te[4], m13 = te[8],
               m21 = te[1], m22 = te[5], m23 = te[9],
               m31 = te[2], m32 = te[6], m33 = te[10],

               trace = m11 + m22 + m33;

    if (trace > 0) {

        const auto s = 0.5f / std::sqrt(trace + 1.0f);

        this->w_ = 0.25f / s;
        this->x_ = (m32 - m23) * s;
        this->y_ = (m13 - m31) * s;
        this->z_ = (m21 - m12) * s;

    } else if (m11 > m22 && m11 > m33) {

        const auto s = 2.0f * std::sqrt(1.0f + m11 - m22 - m33);

        this->w_ = (m32 - m23) / s;
        this->x_ = 0.25f * s;
        this->y_ = (m12 + m21) / s;
        this->z_ = (m13 + m31) / s;

    } else if (m22 > m33) {

        const auto s = 2.0f * std::sqrt(1.0f + m22 - m11 - m33);

        this->w_ = (m13 - m31) / s;
        this->x_ = (m12 + m21) / s;
        this->y_ = 0.25f * s;
        this->z_ = (m23 + m32) / s;

    } else {

        const auto s = 2.f * std::sqrt(1.0f + m33 - m11 - m22);

        this->w_ = (m21 - m12) / s;
        this->x_ = (m13 + m31) / s;
        this->y_ = (m23 + m32) / s;
        this->z_ = 0.25f * s;
    }

    this->onChangeCallback_();

    return *this;
}

float Quaternion::angleTo(const Quaternion &q) const{

    return 2 * std::acos( std::abs( std::clamp( this->dot( q ), - 1.0f, 1.0f ) ) );

}

Quaternion &Quaternion::identity() {

    return this->set(0, 0, 0, 1);
}

Quaternion &Quaternion::invert() {

    // Quaternion is assumed to have unit length

    return this->conjugate();
}

Quaternion &Quaternion::conjugate() {

    this->x_ *= -1;
    this->y_ *= -1;
    this->z_ *= -1;

    this->onChangeCallback_();

    return *this;
}

float Quaternion::dot(const Quaternion &v) const {

    return this->x_ * v.x_ + this->y_ * v.y_ + this->z_ * v.z_ + this->w_ * v.w_;
}

float Quaternion::lengthSq() const {

    return this->x_ * this->x_ + this->y_ * this->y_ + this->z_ * this->z_ + this->w_ * this->w_;
}

float Quaternion::length() const {

    return std::sqrt(this->x_ * this->x_ + this->y_ * this->y_ + this->z_ * this->z_ + this->w_ * this->w_);
}

Quaternion &Quaternion::normalize() {

    auto l = length();

    if (l == 0) {

        this->x_ = 0;
        this->y_ = 0;
        this->z_ = 0;
        this->w_ = 1;

    } else {

        l = 1.0f / l;

        this->x_ = this->x_ * l;
        this->y_ = this->y_ * l;
        this->z_ = this->z_ * l;
        this->w_ = this->w_ * l;
    }

    this->onChangeCallback_();

    return *this;
}

Quaternion &Quaternion::multiply(const Quaternion &q) {

    return this->multiplyQuaternions( *this, q );

}
Quaternion &Quaternion::premultiply(const Quaternion &q) {

    return this->multiplyQuaternions( q, *this );

}
Quaternion &Quaternion::multiplyQuaternions(const Quaternion &a, const Quaternion &b) {

    // from http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/code/index.htm

    const auto qax = a.x_, qay = a.y_, qaz = a.z_, qaw = a.w_;
    const auto qbx = b.x_, qby = b.y_, qbz = b.z_, qbw = b.w_;

    this->x_ = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
    this->y_ = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
    this->z_ = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
    this->w_ = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;

    this->onChangeCallback_();

    return *this;

}

Quaternion &Quaternion::_onChange(std::function<void()> callback) {

    this->onChangeCallback_ = std::move(callback);

    return *this;

}

