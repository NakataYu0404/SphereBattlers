#pragma once
#include <memory>
#include <vector>
#include "../ActorBase.h"

class CollisionManager;


class Stage : public ActorBase
{
public:

	Stage();
	~Stage();

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetParam(void) override;

private:

	std::vector<std::shared_ptr<Transform>> rock_;
};

