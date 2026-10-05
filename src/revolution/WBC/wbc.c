#include <MSL/string.h>

#include <revolution/WBC.h>

void __WBCInit(void) {
    static BOOL IsFirstInitialized;
    s8 prodArea;
    int i;

    if (!IsFirstInitialized) {
        memset(&wbc, 0, 224);
        //! NOTE: This is wrong, data should be set manually
        for (i = 0; i < 11; i++) {
            CALB_DATA[i] = NULL;
        }
        //! String is stubbed out for some reason
        OSRegisterVersion(NULL);
    }
    IsFirstInitialized = TRUE;
    prodArea = SCGetProductArea();
    if ((prodArea == (SC_AREA_JPN || SC_AREA_USA)) ||
        (prodArea == (SC_AREA_KOR || SC_AREA_TWN))) {
        wbc_grav_coff = 0.990813732147217f;
    } else if (prodArea == SC_AREA_EUR) {
        wbc_grav_coff = 0.997757613658905f;
    } else if (prodArea == SC_AREA_CHN) {
        wbc_grav_coff = 0.9994894862174988f;
    } else {
        wbc_grav_coff = 0.9990813732147217f;
    }
}

BOOL WBCSetupCalibration(void) {
    wbc_lib_initialized = FALSE;
    __WBCInit();
    memset(&rxCalib, 0, 16);
    return WPADGetBLCalibration(3, &rxCalib[0], 36, 16, get_calibration1) == 0;
}

BOOL WBCGetCalibrationStatus(void) {
    return wbc_lib_initialized;
}

enum WBCResult WBCSetZEROPoint(f64* pressVal, u32 size) {
    if (!wbc_lib_initialized) {
        return WBC_ERR_BUSY;
    }

    if (size < 4 || !pressVal) {
        return WBC_ERR_INVALID;
    }
    wbc[6] = pressVal[0];
    wbc[13] = pressVal[1];
    wbc[20] = pressVal[2];
    wbc[27] = pressVal[3];
    return WBC_ERR_OK;
}

s32 WBCRead(WPADBLStatus* pStatus, f64 weight[], u32 size) {
    u16 absWeight;
    int i;
    f64 weightCalc;
    u16 calbData;

    if (!wbc_lib_initialized) {
        return WBC_ERR_BUSY;
    }

    if (size < 4 || !pStatus || !weight) {
        return WBC_ERR_INVALID;
    }

    for (i = 4; i != 0; --i) {
        u32 comp = pStatus->press[0] > CALB_DATA[1];
        if (pStatus->press[0] > CALB_DATA[2]) {
            //! TODO(texline): What?
            ++comp;
        }
        if (comp == 2) {
            comp = 1;
        }

        weightCalc = comp - wbc[comp + 3] - wbc[6];
        //! TODO(texline): Fakematch!
        if (wbc[comp] == 0.0f || weightCalc == 0.0f) {
            *weight = 0.0f;
        } else {
            *weight = weightCalc / wbc[comp] / 1000.0f;
        }

        calbData = wbc[0];
        //! As far as I know, this saves the status as the x-acceleration???
        //! Nothing is done with this value either... what?
        pStatus += 2;
        wbc[0] = wbc[2];
        wbc[0] = wbc[7];
        ++weight;
        absWeight += pStatus->press[0] - wbc[0];
    }
    __abs_weight = absWeight;
    //! TODO(texline): Fakematch! Also just wrong
    return (((absWeight ^ 1000) >> 1) - ((absWeight ^ 1000) & absWeight)) >> 31;
}
