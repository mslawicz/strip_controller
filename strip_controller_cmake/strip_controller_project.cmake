#eyJzdGF0ZSI6eyJidWlsZFByZXNldHMiOnsiZGVmYXVsdF9jb25maWciOnsibmFtZSI6ImRlZmF1bHRfY29uZmlnIiwiZGVmaW5pdGlvbnMiOlt7ImtleSI6IkNISVBfREVWSUNFX0NPTkZJR19URVNUX1ZFTkRPUl9OQU1FIiwidmFsIjoiXCJNUyBjb250cm9sbGVyc1wiIn0seyJrZXkiOiJDSElQX0RFVklDRV9DT05GSUdfVEVTVF9QUk9EVUNUX05BTUUiLCJ2YWwiOiJcIkFSR0IgY29udHJvbGxlclwiIn1dLCJyZW1vdmVkRmxhZ3MiOnsiQyI6WyItT3MiXSwiQ1hYIjpbIi1PcyJdLCJBU00iOltdfSwiYWRkaXRpb25hbEZsYWdzIjp7IkMiOlsiLU8wIl0sIkNYWCI6WyItTzAiXSwiQVNNIjpbXX19fSwiY3VycmVudENvbmZJbmRleCI6MH0sImZvbGRlcnMiOltdLCJmaWxlcyI6W119
add_compile_definitions(
	$<$<CONFIG:default_config>:CHIP_DEVICE_CONFIG_TEST_VENDOR_NAME="MS controllers">
	$<$<CONFIG:default_config>:CHIP_DEVICE_CONFIG_TEST_PRODUCT_NAME="ARGB controller">
)

get_target_property(interface_compile_options slc_strip_controller INTERFACE_COMPILE_OPTIONS)
	list(REMOVE_ITEM interface_compile_options $<$<AND:$<CONFIG:default_config>,$<COMPILE_LANGUAGE:C>>:-Os>)
	list(REMOVE_ITEM interface_compile_options $<$<AND:$<CONFIG:default_config>,$<COMPILE_LANGUAGE:CXX>>:-Os>)
set_target_properties(strip_controller PROPERTIES INTERFACE_COMPILE_OPTIONS "${interface_compile_opitons}")

target_compile_options(strip_controller PRIVATE
	$<$<AND:$<CONFIG:default_config>,$<COMPILE_LANGUAGE:C>>:-O0>
	$<$<AND:$<CONFIG:default_config>,$<COMPILE_LANGUAGE:CXX>>:-O0>
)
