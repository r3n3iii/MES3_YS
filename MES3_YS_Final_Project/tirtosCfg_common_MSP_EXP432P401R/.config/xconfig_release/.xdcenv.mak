#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source;C:/ti/simplelink_msp432p4_sdk_3_40_00_05/kernel/tirtos/packages;E:/Documents/Courses/MakingEmbeddedSystems/git_homework/MES3_YS_Final_Project/tirtosCfg_common_MSP_EXP432P401R/.config
override XDCROOT = C:/ti/ccs901/xdctools_3_60_01_27_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source;C:/ti/simplelink_msp432p4_sdk_3_40_00_05/kernel/tirtos/packages;E:/Documents/Courses/MakingEmbeddedSystems/git_homework/MES3_YS_Final_Project/tirtosCfg_common_MSP_EXP432P401R/.config;C:/ti/ccs901/xdctools_3_60_01_27_core/packages;..
HOSTOS = Windows
endif
