#ifndef BUILD_H
#define BUILD_H

#include "config.h"

class Build {
    public:
        Build() = delete;

        bool Execute(
            const ConfigValues& config
        );
        
    private:
        void GenerateNinjaFile(
          const ConfigValues& config,
          const std::string& compiler  
        );

        bool ExecuteNinja();

        bool BuildWithGCC(
            const ConfigValues& config
        );

        bool BuildWithClang(
            const ConfigValues& config
        );

        bool BuildWithMSVC(
            const ConfigValues& config
        );

        bool BuildWithDefault(
            const ConfigValues& config
        );


};

#endif