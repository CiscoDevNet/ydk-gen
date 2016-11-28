# - Try to find libnetconf
# Once done this will define
#
#  LIBNETCONF_FOUND - system has libnetconf
#  LIBNETCONF_INCLUDE_DIRS - the libnetconf include directory
#  LIBYNETCONF_OBJS - Link these to use LibSSH
#

if (LIBNETCONF_LIBRARIES AND LIBNETCONF_INCLUDE_DIRS)
  # in cache already
  set(LIBNETCONF_FOUND TRUE)
else (LIBNETCONF_LIBRARIES AND LIBNETCONF_INCLUDE_DIRS)

    find_path(LIBNETCONF_INCLUDE_DIR
    NAMES
      libnetconf/callbacks.h
      libnetconf/callbacks_ssh.h
      libnetconf/callhome.h
      libnetconf/datastore.h
      libnetconf/datastore_custom.h
      libnetconf/datastore_xml.h
      libnetconf/error.h
      libnetconf/messages.h
      libnetconf/messages_xml.h
      libnetconf/netconf.h
      libnetconf/notifications.h
      libnetconf/notifications_xml.h
      libnetconf/session.h
      libnetconf/transapi.h
      libnetconf/transport.h
      libnetconf/url.h
      libnetconf/with_defaults.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_INCLUDE_PATH}
      ${CMAKE_INSTALL_PREFIX}/include
  )
  
  find_library(LIBNETCONF_LIBRARY
    NAMES
      libnetconf.a
    PATHS
      /usr/lib
      /usr/lib64
      /usr/local/lib
      /usr/local/lib64
      /opt/local/lib
      /sw/lib
      ${CMAKE_LIBRARY_PATH}
      ${CMAKE_INSTALL_PREFIX}/lib
  )

  if (LIBNETCONF_INCLUDE_DIR AND LIBNETCONF_LIBRARY)
      set(LIBNETCONF_FOUND TRUE)
  else (LIBNETCONF_INCLUDE_DIR AND LIBNETCONF_LIBRARY)
      set(LIBNETCONF_FOUND FALSE)
  endif (LIBNETCONF_INCLUDE_DIR AND LIBNETCONF_LIBRARY)

  set(LIBNETCONF_INCLUDE_DIRS ${LIBNETCONF_INCLUDE_DIR})
  set(LIBNETCONF_LIBRARIES ${LIBNETCONF_LIBRARY})

  # show the LIBNETCONF_INCLUDE_DIRS and LIBNETCONF_LIBRARIES variables only in the advanced view
  mark_as_advanced(LIBNETCONF_INCLUDE_DIRS LIBNETCONF_LIBRARIES)

  set(LIBNETCONF_OBJS 
      callbacks.o 
      datastore_custom.o
      edit_config.o 
      messages.o 
      session.o
      transport.o 
      xmldiff.o 
      compat.o
      datastore_empty.o 
      error.o  
      nacm.o
      ssh.o 
      url.o
      yinparser.o
      datastore.o
      datastore_file.o 
      internal.o
      notifications.o  
      transapi.o 
      with_defaults.o)

endif (LIBNETCONF_LIBRARIES AND LIBNETCONF_INCLUDE_DIRS)

