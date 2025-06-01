    #ifndef DIRECTORY_H
    #define DIRECTORY_H

    #include <iostream>
    #include <filesystem>
    #include <vector>
    #include <unordered_set>

    namespace fs = std::filesystem;

    class DirectoryManager
    {
    public:
        static const std::unordered_set<std::string> supportedCommands;
        static const std::unordered_set<std::string>& getSupportedCommands();

        void copyDirectory(const std::vector<std::string> &args)
        {
            if (args.size() != 2)
            {
                printUsage("copy <source_directory> <destination_directory>");
                return;
            }

            fs::path src = args[0];
            fs::path dst = args[1];

            try
            {
                if (!fs::exists(src) || !fs::is_directory(src))
                {
                    std::cerr << "Source directory does not exist or is not a directory:" << src << std::endl;
                    return;
                }
                if (fs::exists(dst)){
                    std::cerr << "Destination directory already exists:" << dst << std::endl;
                    return;
                }

                fs::copy(src, dst, fs::copy_options::recursive);
                std::cout << "Directory copied from " << src << " to " << dst << std::endl;
            } catch (const fs::filesystem_error &e)
            {
                std::cerr << "Error copying directory: " << e.what() << std::endl;
            }
        }

        void createDirectory(const std::vector<std::string>& args){
            if  (args.size() != 1){
                printUsage("create_dir <directory_name>");
                return;
            }

            fs::path dir = args[0];

            try{
                if (fs::exists(dir)){
                    std::cerr << "Directory already exists: " << dir << std::endl;
                    return;
                }

                fs::create_directory(dir);
                std::cout << "Directory created: " << dir << std::endl;
            } catch (const fs::filesystem_error& e){
                std::cerr << "Error creating directory: " << e.what() << std::endl;
            }
        }

        void deleteDirectory(const std::vector<std::string>& args){
            if (args.size() != 1){
                printUsage("delete_dir <directory_name>");
                return;
            }

            fs::path dir = args[0];

            try{
                if (!fs::exists(dir)){
                    std::cerr << "Directory does not exist: " << dir << std::endl;
                    return;
                }

                fs::remove_all(dir);
                std::cout << "Directory deleted: " << dir << std::endl;
            } catch (const fs::filesystem_error& e){
                std::cerr << "Error deleting directory: " << e.what() << std::endl;
            }
        }

        void listDirectoryTree(const std::vector<std::string>& args){
            if (args.size() != 1){
                printUsage("list_dir_tree <directory_name>");
                return;
            }

            fs::path dir = args[0];

            if (!fs::exists(dir)){
                std::cerr << "Directory does not exist: " << dir << std::endl;
                return;
            }

            std::cout << dir.string() << std::endl;
            listDirectoryTreeHelper(dir, 0);
        }

        void moveDirectory(const std::vector<std::string>& args){
            if (args.size() != 2){
                printUsage("move_dir <source_directory> <destination_directory>");
                return;
            }

            fs::path src = args[0];
            fs::path dst = args[1];

            try{
                if (!fs::exists(src)){
                    std::cerr << "Source directory does not exist: " << src << std::endl;
                    return;
                }

                fs::rename(src, dst);
                std::cout << "Directory moved from " << src << " to " << dst << std::endl;
            } catch (const fs::filesystem_error& e){
                std::cerr << "Filesystem error: " << e.what() << std::endl;
            }
        }

        void changeDirectory(const std::vector<std::string>& args) {
            if (args.size() != 1) {
                printUsage("change_dir <directory>");
                return;
            }

            namespace fs = std::filesystem;
            fs::path target = args[0];
            fs::path newDir;

            if (target.is_absolute()) {
                // If they gave an absolute path, use it
                newDir = target;
            } else {
                // For relative paths, resolve from current directory
                newDir = fs::current_path() / target;
            }

            // Clean up any "../" or "./" in the middle
            newDir = newDir.lexically_normal();

            // Now check it
            if (!fs::exists(newDir) || !fs::is_directory(newDir)) {
                std::cerr << "Target directory does not exist or is not a directory: "
                          << newDir << "\n";
                return;
            }

            // Finally, perform the switch
            try {
                fs::current_path(newDir);
                std::cout << "Changed current directory to: "
                          << fs::current_path() << "\n";
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error changing directory: " << e.what() << "\n";
            }
        }

        private:
            static void printUsage(const std::string &usage)
            {
                std::cerr << "Usage: " << usage << std::endl;
            }

            void listDirectoryTreeHelper(const fs::path& path, int level){
                try {
                    if (!fs::exists(path) || !fs::is_directory(path)){
                        return;
                    }

                    for (const auto& entry : fs::directory_iterator(path)){
                        for (int i = 0; i < level; ++i){
                            std::cout << " ";
                        }
                        std::cout << entry.path().filename().string() << std::endl;

                        if (entry.is_directory()){
                            listDirectoryTreeHelper(entry.path(), level + 1);
                        }
                    }
                } catch (const fs::filesystem_error& e){
                    std::cerr << "Error listing directory: " << e.what() << std::endl;
                }
            }
        };

    #endif
