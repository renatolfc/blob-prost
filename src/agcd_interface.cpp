#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>

#include  <random>
#include  <iterator>

#include <ale_interface.hpp>
#include "agcd_interface.hpp"

#ifdef WIN32
const char path_separator = '\\';
#else
const char path_separator = '/';
#endif

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

static inline std::vector<std::string> listdir(const char *path) {
    DIR *dir;
    struct dirent *ent;
    std::string prefix(path);
    std::vector<std::string> ret;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.')
                continue;
            std::string filename(ent->d_name);
            ret.push_back(prefix + "/" + filename);
        }
        closedir(dir);
    } else {
        perror("Could not open directory");
    }

    return ret;
}

static inline bool file_exists(const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

static std::string abspath(const std::string &path) {
    if (path.find(path_separator) == 0) {
        return path;
    }
    char *cwd = get_current_dir_name();
    std::string ret = std::string(cwd) + path_separator + path;
    free(cwd);
    return ret;
}

static inline int episode_number(const std::string &path) {
    size_t offset;
    for (offset = path.size() - 1;
            offset != 0 && path[offset] != path_separator;
            offset--
        );
    if (!offset) {
        return -1;
    }
    return atoi(path.c_str() + offset + 1);
}

AtariState::AtariState(const std::string &path, bool average) : base_path(abspath(path)),
                                                                current_frame(0)
{
    std::vector<std::string> screens;
	screens = listdir(base_path.c_str());

    auto screen_path = *select_randomly(screens.begin(), screens.end());
    int episode = episode_number(screen_path);
    assert(episode >= 0);

    char episode_str[16];
    sprintf(episode_str, "%d", episode);

    base_path = base_path + path_separator + "trajectories" +
                path_separator + episode_str + ".txt";

    std::cout << "Reading episode " << base_path << "...";

    char *tmp = strdup(base_path.c_str());
    if (base_path.size() && tmp[base_path.size()-1] == '/') {
        tmp[base_path.size()-1] = '\0';
    }
    strcpy(base_name, basename(tmp));

    // Kill the file extension
    register size_t i;
    for (i = strlen(base_name) - 1; base_name[i] != '.' && i > 0; i--);
    base_name[i] = '\0';

    read_data();

    std::cout << " done! " << std::endl;
}

inline static char *get_line(char *str, size_t strsize, FILE *fp) {
    register int c;
    register size_t offset = 0;
    while (((c = fgetc(fp)) != EOF) && offset < strsize - 1) {
        str[offset++] = c;
        if (c == '\n') {
            break;
        }
    }
    str[offset] = '\0';
    if (c == EOF) {
        return NULL;
    } else {
        return str;
    }
}

inline void AtariState::read_data() {
    char c;
    char buffer[512];
    FILE *fp = fopen(base_path.c_str(), "r");
    /* skip 2 lines {{{ */
    get_line(buffer, 512, fp);
    get_line(buffer, 512, fp);
    /* }}} */
    std::string last_accessible_path;

    int i;
    size_t j = 0;
    while (get_line(buffer, 512, fp) != NULL) {
        char tmp[16];
        agcd_frame_t current;
        i = sscanf(buffer, "%d,%d, %d, %s %d\n",
               &current.frame,
               &current.reward,
               &current.score,
               &tmp,
               &current.action
        );
        if (i == EOF) {
            continue;
        }
        if (strncmp(tmp, "True", 4) == 0) {
            current.terminal = true;
        } else {
            current.terminal = false;
        }
        current.frame += 1;
        frames.push_back(current);
    }
    fclose(fp);
}

size_t AtariState::getCurrentFrame() {
    return current_frame;
}

Action AtariState::getCurrentAction() {
    return frames[current_frame].action;
}

Action AtariState::getNextAction() {
    if (current_frame < frames.size() - 2) {
        return frames[current_frame + 1].action;
    } else {
        return PLAYER_A_NOOP;
    }
}

reward_t AtariState::getNextReward() {
    return frames[current_frame].reward;
}

bool AtariState::isTerminal() {
    return current_frame == frames.size() - 1;
}

void AtariState::step() {
    if (current_frame < frames.size() - 1) {
        current_frame += 1;
    }
}
