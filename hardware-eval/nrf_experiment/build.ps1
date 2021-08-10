
$env:ZEPHYR_TOOLCHAIN_VARIANT = "gnuarmemb"; 
$env:GNUARMEMB_TOOLCHAIN_PATH = "C:\gnuarmemb"; 
west build -p auto -b nrf52840dk_nrf52840 prog/taiko-hid
