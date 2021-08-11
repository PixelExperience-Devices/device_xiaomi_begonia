#!/vendor/bin/sh

# change partition permission
/vendor/bin/chown root:system /mtd@preloader
/vendor/bin/chmod 0640 /mtd@preloader
/vendor/bin/chown root:system /mtd@pro_info
/vendor/bin/chmod 0660 /mtd@pro_info
/vendor/bin/chown root:system /mtd@bootimg
/vendor/bin/chmod 0640 /mtd@bootimg
/vendor/bin/chown root:system /mtd@recovery
/vendor/bin/chmod 0640 /mtd@recovery
/vendor/bin/chown root:system /mtd@sec_ro
/vendor/bin/chmod 0640 /mtd@sec_ro
/vendor/bin/chown root:system /mtd@nvram
/vendor/bin/chmod 0660 /mtd@nvram
/vendor/bin/chown root:system /mtd@seccfg
/vendor/bin/chmod 0660 /mtd@seccfg
/vendor/bin/chown root:system /mtd@misc
/vendor/bin/chmod 0660 /mtd@misc
