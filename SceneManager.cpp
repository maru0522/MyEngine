#include "SceneManager.h"

SceneManager::SceneManager(void)
{
    sceneFactory_ = std::make_unique<SceneFactory>();
}

SceneManager::~SceneManager(void)
{
    if (currentScene_) {
        currentScene_->Finalize();
        currentScene_.reset();
    }
}

void SceneManager::RequestChangeScene(std::unique_ptr<IScene>& nextScene, int32_t waitFrame)
{
    nextScene_ = std::move(nextScene);
    waitFrame_ = waitFrame;
}

void SceneManager::Initialize(SceneFactory::Usage firstScene)
{
    // �ŏ��̃V�[���𐶐�
    currentScene_ = sceneFactory_->CreateScene(firstScene);
    currentScene_->Initialize();
}

void SceneManager::Update(void)
{
    // ���V�[���̗\�񂪂���
    if (nextScene_) {
        // �ҋ@�t���[���w�肪�Ȃ� && ���݃V�[����nullptr�ł͂Ȃ�
        if (waitFrame_ == 0 && currentScene_) {
            currentScene_->Finalize();
            currentScene_.reset();
        }

        // �V�[���ڍs
        currentScene_ = std::move(nextScene_); // �Ǘ������ڏ�
        nextScene_.reset();                    // ���V�[����nullptr�ɂ���
        currentScene_->Initialize();           // ���݃V�[����������
    }

    // ���݃V�[��Update()
    currentScene_->Update();


    // �ҋ@�t���[��������������
    waitFrame_--;
    // �ҋ@�t���[����0�ȉ��ɂȂ�Ȃ�
    waitFrame_ = (std::max)(waitFrame_, 0);
}

void SceneManager::Draw3d(void)
{
    currentScene_->Draw3d();
}

void SceneManager::Draw2d(void)
{
    currentScene_->Draw2d();
}
