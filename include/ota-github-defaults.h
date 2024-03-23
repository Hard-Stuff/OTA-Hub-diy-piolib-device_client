#pragma once

#ifndef OTAGH_SERVER
#define OTAGH_SERVER "api.github.com"
#endif

#ifndef OTAGH_PORT
#define OTAGH_PORT 443
#endif

#ifndef OTAGH_CHECK_PATH
#define OTAGH_CHECK_PATH "/repos/" OTAGH_OWNER_NAME "/" OTAGH_REPO_NAME "/releases/latest" // TODO:: would this work?
#endif

#ifndef OTAGH_BIN_PATH
#define OTAGH_BIN_PATH "/repos/" OTAGH_OWNER_NAME "/" OTAGH_REPO_NAME "/releases/assets/"
#endif
