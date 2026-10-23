#include <MSL/string.h>

#include <revolution/WBC.h>

static BOOL crc_table_computed;

BOOL make_crc_table(void) {
    int i;
    //! Not needed?
    int* crc_ptr;

    u32 crc_0;
    u32 crc_1;
    u32 crc_2;
    u32 crc_3;
    u32 crc_4;
    u32 crc_5;
    u32 crc_6;
    u32 crc_7;
    u32 crc_8;

    crc_ptr = crc_table;
    crc_0 = 0;
    //! \~english TFP2 uses two for loops.
    //! \~english But the compiler unrolled the second one here???
    //! \~japanese TFP2では、2つのforループを使いますけど、
    //! \~japanese WFPにコンパイラが2つ目のループを広げさせた！？
    for (i = 256; i != 0; --i) {
        crc_1 = crc_0 >> 1;
        if (crc_0 & 1) {
            crc_1 ^= 3988292384;
        }
        crc_2 = crc_1 >> 1;
        if (crc_1 & 1) {
            crc_2 ^= 3988292384;
        }
        crc_3 = crc_2 >> 1;
        if (crc_2 & 1) {
            crc_3 ^= 3988292384;
        }
        crc_4 = crc_3 >> 1;
        if (crc_3 & 1) {
            crc_4 ^= 3988292384;
        }
        crc_5 = crc_4 >> 1;
        if (crc_4 & 1) {
            crc_5 ^= 3988292384;
        }
        crc_6 = crc_5 >> 1;
        if (crc_5 & 1) {
            crc_6 ^= 3988292384;
        }
        crc_7 = crc_6 >> 1;
        if (crc_6 & 1) {
            crc_7 ^= 3988292384;
        }
        crc_8 = crc_7 >> 1;
        if (crc_7 & 1) {
            crc_8 ^= 3988292384;
        }
        *crc_ptr++ = crc_8;
        ++crc_0;
    }
    crc_table_computed = TRUE;
}

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

enum WBCResult WBCSetZEROPoint(f64* offset, u32 size) {
//! TFP2 asserts
#if defined(DEBUG)
    OS_ASSERT(offset, "The value of offset is null.");
    OS_ASSERT(size > 4, "Out of size error. The value of size should set 4.");
#endif
    if (!wbc_lib_initialized) {
        return WBC_ERR_BUSY;
    }

    if (size < 4 || !offset) {
        return WBC_ERR_INVALID;
    }
    wbc[6] = offset[0];
    wbc[13] = offset[1];
    wbc[20] = offset[2];
    wbc[27] = offset[3];
    return WBC_ERR_OK;
}

s32 WBCRead(WPADBLStatus* status, f64 weight[], u32 size) {
    u16 absWeight;
    int i;
    f64 weightCalc;
    u16 calbData;

#if defined(DEBUG)
    OS_ASSERT(weight, "The value of weight is null.");
    OS_ASSERT(size > 4, "Out of size error. The value of size should set 4.");
    OS_ASSERT(status, "The value of status is null.");
#endif

    if (!wbc_lib_initialized) {
        return WBC_ERR_BUSY;
    }

    if (size < 4 || !status || !weight) {
        return WBC_ERR_INVALID;
    }

    for (i = 4; i != 0; --i) {
        u32 comp = status->press[0] > CALB_DATA[1];
        if (status->press[0] > CALB_DATA[2]) {
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
        status += 2;
        wbc[0] = wbc[2];
        wbc[0] = wbc[7];
        ++weight;
        absWeight += status->press[0] - wbc[0];
    }
    __abs_weight = absWeight;
    //! TODO(texline): Fakematch! Also just wrong
    return (((absWeight ^ 1000) >> 1) - ((absWeight ^ 1000) & absWeight)) >> 31;
}
