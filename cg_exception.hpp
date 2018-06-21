#pragma once

class FileNotExistsError: public std::exception {
public:
    FileNotExistsError() = delete;
    FileNotExistsError(const std::string &path) {
    	error_message = "[file error] File not exists at " + path;
    }
    const char *what() const noexcept {
        return error_message.c_str();
    }
private:
    std::string error_message;
};

class ShaderCompileError: public std::exception {
public:
	ShaderCompileError() = delete;
	ShaderCompileError(const std::string &log) {
		error_message = "[shader compile error] " + log;
	}
	const char *what() const noexcept {
		return error_message.c_str();
	}
private:
	std::string error_message;
};

class ShaderLinkError: public std::exception {
public:
	ShaderLinkError() = delete;
	ShaderLinkError(const std::string &log) {
		error_message = "[shader link error] " + log;
	}
	const char *what() const noexcept {
		return error_message.c_str();
	}
private:
	std::string error_message;
};

class LoadPictureError: public std::exception {
public:
	LoadPictureError() = delete;
	LoadPictureError(const std::string &path) {
		error_message = "[picture format error] Fail to load picture at " + path;
	}
	const char *what() const noexcept {
		return error_message.c_str();
	}
private:
	std::string error_message;
};

class AssimpError: public std::exception {
public:
	AssimpError() = delete;
	AssimpError(const std::string &error_string) {
		error_message = "[assimp error] " + error_string;
	}
	const char *what() const noexcept {
		return error_message.c_str();
	}
private:
	std::string error_message;
};

class ShaderSettingError: public std::exception {
public:
	ShaderSettingError() = delete;
	ShaderSettingError(const std::string &name) {
		error_message = "[shader setting error] Fail to set uniform variable " + name;
	}
	const char *what() const noexcept {
		return error_message.c_str();
	}
private:
	std::string error_message;
};