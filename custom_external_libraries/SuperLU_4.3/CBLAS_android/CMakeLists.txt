set(headers
    f2c.h
    slu_Cnames.h
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(sources input_error.c)
else()
  set(sources "")
endif()

#if (enable_single)
    list(APPEND sources
      isamax.c
      sasum.c
      saxpy.c
      scopy.c
      sdot.c
      snrm2.c
      srot.c
      sscal.c
      sswap.c
      sgemv.c
      ssymv.c
      strsv.c
      sger.c
      ssyr2.c
    )
#endif()

#if (enable_double)
    list(APPEND sources
      idamax.c
      dasum.c
      daxpy.c
      dcopy.c
      ddot.c
      dnrm2.c
      drot.c
      dscal.c
      dswap.c
      dgemv.c
      dsymv.c
      dtrsv.c
      dger.c
      dsyr2.c
    )
#endif()

#if (enable_complex)
    list(APPEND sources
      icamax.c
      scasum.c
      caxpy.c
      ccopy.c
      scnrm2.c
      cscal.c
      cswap.c
      cdotc.c
      cgemv.c
      chemv.c
      ctrsv.c
      cgerc.c
      cher2.c
    )
#endif()

#if (enable_complex16)
    list(APPEND sources
      izamax.c
      dzasum.c
      zaxpy.c
      zcopy.c
      dznrm2.c
      zscal.c
      dcabs1.c
      zswap.c
      zdotc.c
      zgemv.c
      zhemv.c
      ztrsv.c
      zgerc.c
      zher2.c
    )
#endif()

add_library(cblas ${sources})

include(GNUInstallDirs)
install(TARGETS blas
  EXPORT blasTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(EXPORT blasTargets
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/superlu"
  )
export(EXPORT blasTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/blasTargets.cmake"
  )
