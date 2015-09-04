#pragma once
#include <vector>
#include <math.h>
#include <cassert>
#include <QDebug>

class Hand{
    typedef double Real;
    typedef unsigned int uint;
public:
    /// Names to acces pars by group
    enum PARAMETER_GROUP{
        POSITION = 0,   /// [x,y,z]
        ROTATION = 3,   /// [x, i,j,k]
        FINGER   = 7,   /// [a1,a2,a3,t1]
        MIDDLE   = 11,  /// [a1,a2,a3,t1]
        ANULAR   = 15,  /// [a1,a2,a3,t1]
        PINKY    = 19,  /// [a1,a2,a3,t1]
        THUMB    = 23   /// [????]
    };

    /// Names to acces pars directly
    enum PARAMETER{
        /// FINGER
        FINGER_BEND_0 = FINGER+0,
        FINGER_BEND_1 = FINGER+1,
        FINGER_BEND_2 = FINGER+2,
        FINGER_TILT   = FINGER+3,
        /// MIDDLE
        MIDDLE_BEND_0 = MIDDLE+0,
        MIDDLE_BEND_1 = MIDDLE+1,
        MIDDLE_BEND_2 = MIDDLE+2,
        MIDDLE_TILT   = MIDDLE+3,
        /// ANULAR
        ANULAR_BEND_0 = ANULAR+0,
        ANULAR_BEND_1 = ANULAR+1,
        ANULAR_BEND_2 = ANULAR+2,
        ANULAR_TILT   = ANULAR+3,
        /// PINKY
        PINKY_BEND_0 = PINKY+0,
        PINKY_BEND_1 = PINKY+1,
        PINKY_BEND_2 = PINKY+2,
        PINKY_TILT   = PINKY+3,
        ///THUMB
        THUMB_BEND_0 = THUMB+0,
        THUMB_BEND_1 = THUMB+1,
        THUMB_TILT   = THUMB+2
    };

public:
    Hand(){
        __pars.resize(32,0);
        PI = acos(-1.0);
        DEG_TO_RAD = PI/180.0;
        RAD_TO_DEG = 180.0/PI;

        for(uint i=0; i<__pars.size(); i++){
            __pars[i] = default_normalized(PARAMETER(i));
            // qDebug() << __pars[i];
        }
    }

    virtual void init(){} ///< renderer initialization
    virtual void draw() = 0; ///< renderer main function

private:
    /// Stores generic model parameters **intrinsically** normalized [0,1]
    std::vector<Real> __pars; ///< 32
    Real PI;
    Real DEG_TO_RAD;
    Real RAD_TO_DEG;

public:
    /// allows to do getParsNormalized(FINGER)[0]
    Real* pars(PARAMETER_GROUP par_id){ return &__pars[par_id]; }
    Real& par(PARAMETER par_id){ return __pars[par_id]; }

    Real par_unnorm(PARAMETER par_id){
        double mypar = par(par_id);
        double mypar_unnorm = norm_to_unnorm(par_id, mypar);
        return mypar_unnorm;
    }

    /// Converts unnormalized parameter to normalized
    Real unnorm_to_norm(PARAMETER par_id, Real unnorm){
        Range r = range(par_id); /// [min,max]
        return (unnorm - r.min) / (r.max - r.min);
    }
    Real norm_to_unnorm(PARAMETER par_id, Real norm){
        Range r = range(par_id); /// [min,max]
        return (1-norm)*r.min + (norm)*r.max;
    }

    struct Range{
        Real min, max;
        Range(Real min, Real max) : min(min), max(max){}
    };

    Real default_normalized(PARAMETER par_id){
        Real val = default_unnormalized(par_id);
        return unnorm_to_norm(par_id,val);
    }

    Real default_unnormalized(PARAMETER par){
        /// Just makes sense that by default the
        /// angles w.r.t. default are zero
        switch(par){
            case FINGER_BEND_0: return 40;
            case FINGER_BEND_1: return 20;
            default: return 0;
        }
        assert(false);
    }

    /// For each parameter, defines the range of the values that will be linearly
    /// mapped to the [0,1] range
    Range range(PARAMETER par){
        switch(par){
            /// FINGER
            case FINGER_BEND_0: return Range( -10,  85);
            case FINGER_BEND_1: return Range(   0, 110);
            case FINGER_BEND_2: return Range(  -5,  70);
            case FINGER_TILT:   return Range( -10,   5);
            /// MIDDLE
            case MIDDLE_BEND_0: return Range( -10,  85);
            case MIDDLE_BEND_1: return Range(   0, 110);
            case MIDDLE_BEND_2: return Range(  -5,  70);
            case MIDDLE_TILT:   return Range( -10,   5);
            /// ANULAR
            case ANULAR_BEND_0: return Range( -10,  85);
            case ANULAR_BEND_1: return Range(   0, 110);
            case ANULAR_BEND_2: return Range(  -5,  70);
            case ANULAR_TILT:   return Range( -5,   10);
            /// PINKY
            case PINKY_BEND_0: return Range( -10,  85);
            case PINKY_BEND_1: return Range(   0, 110);
            case PINKY_BEND_2: return Range(  -5,  70);
            case PINKY_TILT:   return Range( -5,   10);
            /// THUMB
            case THUMB_BEND_0: return Range( -10,  85);
            case THUMB_BEND_1: return Range(   0, 110);
            case THUMB_TILT:   return Range( -10,   5);

            default:            return Range(   0,   1);
        }
    }

    QString name(PARAMETER par){
        switch(par){
            /// FINGER
            case FINGER_BEND_0: return "Finger Bend 0";
            case FINGER_BEND_1: return "Finger Bend 1";
            case FINGER_BEND_2: return "Finger Bend 2";
            case FINGER_TILT:   return "Finger Tilt  ";
            /// MIDDLE
            case MIDDLE_BEND_0: return "Middle Bend 0";
            case MIDDLE_BEND_1: return "Middle Bend 1";
            case MIDDLE_BEND_2: return "Middle Bend 2";
            case MIDDLE_TILT:   return "Middle Tilt";
            /// ANULAR
            case ANULAR_BEND_0: return "Anular Bend 0";
            case ANULAR_BEND_1: return "Anular Bend 1";
            case ANULAR_BEND_2: return "Anular Bend 2";
            case ANULAR_TILT:   return "Anular Tilt";
            /// PINKY
            case PINKY_BEND_0: return "Pinky Bend 0";
            case PINKY_BEND_1: return "Pinky Bend 1";
            case PINKY_BEND_2: return "Pinky Bend 2";
            case PINKY_TILT:   return "Pinky Tilt";
            /// THUMB
            case THUMB_BEND_0: return "Thumb Bend 0";
            case THUMB_BEND_1: return "Thumb Bend 1";
            case THUMB_TILT:   return "Thumb Tilt";

            default:            return "WRITE NAME   ";
        }
    }
};
