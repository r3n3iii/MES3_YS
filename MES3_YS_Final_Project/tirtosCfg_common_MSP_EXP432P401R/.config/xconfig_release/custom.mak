## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4f linker.cmd package/cfg/release_pem4f.oem4f

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/release_pem4f.xdl
	$(SED) 's"^\"\(package/cfg/release_pem4fcfg.cmd\)\"$""\"C:/Users/Rene/workspace_v9/PINV15-190/tirtosCfg_common_MSP_EXP432P401R/.config/xconfig_release/\1\""' package/cfg/release_pem4f.xdl > $@
	-$(SETDATE) -r:max package/cfg/release_pem4f.h compiler.opt compiler.opt.defs