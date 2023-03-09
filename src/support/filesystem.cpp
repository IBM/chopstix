/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2019 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ----------------------------------------------------------------------------
#
*/
/******************************************************************************
 * NAME        : support/filesystem.cpp
 * DESCRIPTION : Helper functions for filesystem operations.
 ******************************************************************************/

#include "support/filesystem.h"

#include <cstdlib>
#include <fstream>

#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "support/check.h"

namespace chopstix {
namespace filesystem {

// Basic file stats
bool exists(const std::string &path) {
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}

bool isfile(const std::string &path) { return isreg(path) || islink(path); }

bool isreg(const std::string &path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFMT) == S_IFREG;
}

bool islink(const std::string &path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFMT) == S_IFLNK;
}

bool isdir(const std::string &path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFMT) == S_IFDIR;
}

bool isexe(const std::string &cmd) {
    std::string which = "which " + cmd + " >/dev/null 2>&1";
    int ret = system(which.c_str());
    check(ret != -1, "Unable to fork process");
    return ret == 0;
}

// Path operations
std::string realpath(const std::string &path) {
    char *real = ::realpath(path.c_str(), nullptr);
    std::string res = real;
    free(real);
    return res;
}

std::string basename(const std::string &path) {
    if (path.empty()) {
        return "";
    }
    auto pos = path.rfind('/');
    return pos == std::string::npos ? path : path.substr(pos + 1);
}

std::string dirname(const std::string &path) {
    if (path.empty()) {
        return "";
    }
    auto pos = path.rfind('/');
    return pos == std::string::npos ? "" : path.substr(0, pos);
}

std::string rootname(const std::string &path) {
    if (path.empty()) {
        return "";
    }
    auto pos = path.rfind('.');
    return pos == std::string::npos ? path : path.substr(0, pos);
}

std::string extname(const std::string &path) {
    if (path.empty()) {
        return "";
    }
    auto pos = path.rfind('.');
    return pos == std::string::npos ? "" : path.substr(pos + 1);
}

std::pair<std::string, std::string> split(const std::string &path) {
    return std::make_pair(basename(path), dirname(path));
}

std::pair<std::string, std::string> splitext(const std::string &path) {
    return std::make_pair(rootname(path), extname(path));
}

std::string join(std::string path_a, std::string path_b) {
    if (!path_a.empty() && path_a.back() == '/') {
        path_a = path_a.substr(0, path_a.size() - 1);
    }
    if (!path_b.empty() && path_b.front() == '/') {
        path_b = path_b.substr(0, path_b.size() - 1);
    }
    return path_a + '/' + path_b;
}

// File system
void mkdir(std::string path, int mode) {
    if (path.empty() || isdir(path)) {
        return;
    }
    if (path.back() == '/') path.pop_back();
    mkdir(dirname(path), mode);
    check(::mkdir(path.c_str(), mode) == 0, "Cannot create directory '%s'",
          path);
}

void rmdir(const std::string &path) {
    check(::rmdir(path.c_str()) == 0, "Cannot remove directory '%s'", path);
}

void chdir(const std::string &path) {
    check(::chdir(path.c_str()) == 0, "Cannot change directory '%s'", path);
}

void remove(const std::string &path) {
    check(::unlink(path.c_str()) == 0, "Cannot remove file '%s'", path);
}

void rename(const std::string &from, const std::string &to) {
    check(::rename(from.c_str(), to.c_str()) == 0, "Cannot rename file");
}

void copy(const std::string &from, const std::string &to) {
    std::ifstream ifs(from);
    check(ifs.is_open(), "Cannot open file");
    mkdir(dirname(to));
    std::ofstream ofs(to);
    check(ofs.is_open(), "Cannot open file");
    size_t len = 2048;
    char *buf = new char[len];

    while (ifs.read(buf, len)) {
        ofs.write(buf, len);
    }
    if (ifs.gcount()) {
        ofs.write(buf, ifs.gcount());
    }

    delete[] buf;
}

std::vector<std::string> list(const std::string &path) {
    DIR *dir;
    struct dirent *ent;
    dir = opendir(path.c_str());
    check(dir != NULL, "Cannot open directory");
    std::vector<std::string> files;
    while ((ent = readdir(dir)) != NULL) {
        std::string name = ent->d_name;
        if (name.front() != '.') {
            files.push_back(name);
        }
    }
    closedir(dir);
    return files;
}

void deep_remove(const std::string &path) {
    if (isdir(path)) {
        for (auto &ent : list(path)) {
            deep_remove(join(path, ent));
        }
        rmdir(path);
    } else {
        remove(path);
    }
}

void deep_copy(const std::string &from, const std::string &to) {
    if (isdir(from)) {
        mkdir(to);
        for (auto &ent : list(from)) {
            deep_copy(ent, join(to, basename(ent)));
        }
    } else {
        copy(from, to);
    }
}

}  // namespace filesystem
}  // namespace chopstix
