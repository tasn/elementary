/**
 * @defgroup App App
 * @ingroup elm_infra_group
 * @brief This group provides functions to get the application information.
 *
 * @{
 */

/**
 * @brief Provides information in order to make Elementary determine the @b
 *        run time location of the software in question, so other data files
 *        such as images, sound files, executable utilities, libraries,
 *        modules and locale files can be found.
 *
 * @since_tizen 2.3
 *
 * @remarks This function allows one to re-locate the application somewhere
 *          else after compilation, if the developer wishes for easier
 *          distribution of pre-compiled binaries.
 *
 * @remarks The prefix system is designed to locate where the given software is
 *          installed (under a common path prefix) at run time and then report
 *          specific locations of this prefix and common directories inside
 *          this prefix like the binary, library, data, and locale directories,
 *          through the @c elm_app_*_get() family of functions.
 *
 * @remarks Call elm_app_info_set() early, before you change the working
 *          directory or anything about @c argv[0], so it gets accurate
 *          information.
 *
 *          It then tries to trace back which file @a mainfunc comes from,
 *          if provided, to determine the application's prefix directory.
 *
 * @remarks The @a dom parameter provides a string prefix to prepend before
 *          environment variables, allowing a fallback to @b specific
 *          environment variables to locate the software. You would most
 *          probably provide a lowercase string there, because it also
 *          serves as the directory domain, explained next. For environment
 *          variables purposes, this string is made uppercase. For example if
 *          @c "myapp" is provided as the prefix, then the program would expect
 *          @c "MYAPP_PREFIX" as a master environment variable to specify the
 *          exact install prefix for the software, or more specific environment
 *          variables like @c "MYAPP_BIN_DIR", @c "MYAPP_LIB_DIR", @c
 *          "MYAPP_DATA_DIR", and @c "MYAPP_LOCALE_DIR", which could be set by
 *          the user or scripts before launching. If not provided (@c NULL),
 *          environment variables are not used to override compiled-in
 *          defaults or auto detections.
 *
 *          The @a dom string also provides a subdirectory inside the system
 *          shared data directory for data files. For example, if the system
 *          directory is @c /usr/local/share, then this directory name is
 *          appended, creating @c /usr/local/share/myapp, if it @b is @c
 *          "myapp". It is expected that the application installs data files in
 *          this directory.
 *
 * @remarks The @a checkfile is a file name or path of something inside the
 *          share or data directory to be used to test if the prefix
 *          detection worked. For example, your app installs a wallpaper
 *          image as @c /usr/local/share/myapp/images/wallpaper.jpg and so to
 *          check that this worked, provide @c "images/wallpaper.jpg" as the @a
 *          checkfile string.
 *
 * @param[in] mainfunc This is your application's main function name,
 *                 whose binary's location is to be found \n 
 *                 Providing @c NULL makes Elementary not use it
 * @param[in] dom This is used as the application's "domain", in the
 *            form of a prefix to any environment variables that may
 *            override prefix detection and the directory name, inside the
 *            standard share or data directories, where the software's
 *            data files are looked for
 * @param[in] checkfile This is an (optional) magic file's path to check
 *                  for existence (and it must be located in the data directory,
 *                  under the share directory provided above) \n
 *                  Its presence helps determine whther the prefix found is correct \n
 *                  Pass @c NULL if the check is not to be done.
 *
 * @see elm_app_compile_bin_dir_set()
 * @see elm_app_compile_lib_dir_set()
 * @see elm_app_compile_data_dir_set()
 * @see elm_app_compile_locale_set()
 * @see elm_app_prefix_dir_get()
 * @see elm_app_bin_dir_get()
 * @see elm_app_lib_dir_get()
 * @see elm_app_data_dir_get()
 * @see elm_app_locale_dir_get()
 */
EAPI void        elm_app_info_set(void *mainfunc, const char *dom, const char *checkfile);

/**
 * @brief Set a formal name to be used with the elm application.
 *
 * @since_tizen 2.3
 *
 * @param[in] name Application name.
 */
EAPI void        elm_app_name_set(const char *name);

/**
 * @brief Provides information on the @b fallback application's binaries
 *        directory, in scenarios where they get overridden by
 *        elm_app_info_set().
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary also uses this path to determine the actual
 *          names of binaries' directory paths, maybe changing it to be @c
 *          something/local/bin instead of @c something/bin, only, for
 *          example.
 *
 * @remarks You should call this function @b before
 *          elm_app_info_set().
 *
 * @param[in] dir The path to the default binaries directory (compile time
 *            one)
 */
EAPI void        elm_app_compile_bin_dir_set(const char *dir);

/**
 * @brief Provides information on the @b fallback application's libraries
 *        directory, on scenarios where they get overridden by
 *        elm_app_info_set().
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary also uses this path to determine the actual
 *          names of libraries' directory paths, maybe changing it to be @c
 *          something/lib32 or @c something/lib64 instead of @c something/lib
 *          only, for example.
 *
 * @remarks You should call this function @b before
 *          elm_app_info_set().
 *
 * @param[in] dir The path to the default libraries directory (compile
 *            time one)
 */
EAPI void        elm_app_compile_lib_dir_set(const char *dir);

/**
 * @brief Provides information on the @b fallback application's data
 *        directory, on scenarios where they get overridden by
 *        elm_app_info_set().
 *
 * @since_tizen 2.3
 *
 * @remarks Elementary also uses this path to determine the actual
 *          names of data directory paths, maybe changing it to be @c
 *          something/local/share instead of @c something/share only, for
 *          example.
 *
 * @remarks You should call this function @b before
 *          elm_app_info_set().
 *
 * @param[in] dir The path to the default data directory (compile time
 *            one)
 */
EAPI void        elm_app_compile_data_dir_set(const char *dir);

/**
 * @brief Provides information on the @b fallback application's locale
 *        directory, on scenarios where they get overridden by
 *        elm_app_info_set().
 *
 * @since_tizen 2.3
 *
 * @remarks You should call this function @b before
 *          elm_app_info_set().
 *
 * @param[in] dir The path to the default locale directory (compile time
 *            one)
 */
EAPI void        elm_app_compile_locale_set(const char *dir);

/**
 * @brief Retrieve the application formal name, as set by elm_app_name_set().
 *
 * @since_tizen 2.3
 *
 * @return The application formal name.
 */
EAPI const char *elm_app_name_get(void);

/**
 * @brief Retrieves the application's run time prefix directory, as set by
 *        elm_app_info_set() and the way (environment) the application is
 *        run from it.
 *
 * @since_tizen 2.3
 *
 * @return The directory prefix that the application is actually using
 */
EAPI const char *elm_app_prefix_dir_get(void);

/**
 * @brief Retrieves the application's run time binaries prefix directory, as
 *        set by elm_app_info_set() and the way (environment) the application
 *        is run from it.
 *
 * @since_tizen 2.3
 *
 * @return The binaries directory prefix that the application is actually
 *         using
 */
EAPI const char *elm_app_bin_dir_get(void);

/**
 * @brief Retrieves the application's run time libraries prefix directory, as
 *        set by elm_app_info_set() and the way (environment) the application
 *        is run from it.
 *
 * @since_tizen 2.3
 *
 * @return The libraries directory prefix that the application is actually
 *         using
 */
EAPI const char *elm_app_lib_dir_get(void);

/**
 * @brief Retrieves the application's run time data prefix directory, as
 *        set by elm_app_info_set() and the way (environment) the application
 *        is run from it.
 *
 * @since_tizen 2.3
 *
 * @return The data directory prefix that the application is actually
 *         using
 */
EAPI const char *elm_app_data_dir_get(void);

/**
 * @brief Retrieves the application's run time locale prefix directory, as
 *        set by elm_app_info_set() and the way (environment) the application
 *        is run from it.
 *
 * @since_tizen 2.3
 *
 * @return The locale directory prefix the application is actually
 *         using
 */
EAPI const char *elm_app_locale_dir_get(void);

/**
 * @brief Set the base scale of the application.
 *
 * @param base_scale The scale that the application is made on the basis of.
 *
 * @remarks The scale is used for the application to be scaled.
 *          if the application isn't made on the basis of scale 1.0,
 *          the application layout will be break. So if the application set
 *          the base scale, it is applied when the application is scaled.
 *
 * @remarks You should call this function @b before using ELM_SCALE_SIZE macro.
 *
 * @since_tizen 2.3
 */
EAPI void elm_app_base_scale_set(double base_scale);

/**
 * @brief Get the base scale of the application.
 *
 * @return The base scale which the application sets.
 *
 * @since_tizen 2.3
 */
EAPI double elm_app_base_scale_get(void);

/**
 * @}
 */
