function(ADD_EP_VERSION ep version)
    STRING(TOUPPER ext_${ep}_version name)
    SET(${name} ${version} PARENT_SCOPE)
    MESSAGE("-- External Project ${ep} version ... ${version}")
endfunction()

if (APPLE OR WIN32)
    ADD_EP_VERSION(libexiv2 v0.28.3)
    ADD_EP_VERSION(libjpeg-turbo 3.1.0)
    ADD_EP_VERSION(libraw 0.21.3)
    ADD_EP_VERSION(brotli v1.1.0)
    ADD_EP_VERSION(libjxl v0.11.1)
endif()

if (APPLE)
    ADD_EP_VERSION(highway 1.2.0)
endif()

if (WIN32)
    ADD_EP_VERSION(highway 1.1.0)
    ADD_EP_VERSION(zlib v1.3.1)
    ADD_EP_VERSION(libexpat R_2_6_4)
    ADD_EP_VERSION(libde265 v1.0.15)
    ADD_EP_VERSION(libheif v1.19.5)
endif()
