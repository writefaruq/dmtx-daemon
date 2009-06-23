AC_DEFUN([AC_PATH_LIBDMTX], [
    AC_CHECK_LIB(dmtx, dmtxEncodeCreate, dummy=yes,
				AC_MSG_ERROR(libdmtx => 0.7 is required))
    PKG_CHECK_MODULES(
            MAGICK,
            Wand >= 6.2.4,
            true,
            AC_MSG_ERROR(dmtx plug-in requires Wand >= 6.2.4))
    AC_SUBST(MAGICK_CFLAGS)
    AC_SUBST(MAGICK_LIBS)
    AC_SUBST(LIBDMTX_CFLAGS)
    AC_SUBST(LIBDMTX_LIBS)
])
