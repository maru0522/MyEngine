#pragma once
#include <filesystem>

class Pipe
{
public:
    //>> 定義
    using fsPath = std::filesystem::path;

    //>> 関数
    Pipe(const fsPath& arg_modelPath);

    void Update(void);
    void Draw(void);

    //>> 変数

};

