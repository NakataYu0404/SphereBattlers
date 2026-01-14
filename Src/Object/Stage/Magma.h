#pragma once
#include <memory>
#include <vector>
#include "../ActorBase.h"

class EffectController;
class MagmaEffects;

class Magma : public ActorBase
{
public:

	enum class EFFECT_TYPE
	{
		MAGMA_BUBBLE,
		MAGMA_JUMP,
	};

	Magma();
	~Magma();

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetParam(void) override;


private:

	std::vector<std::shared_ptr<MagmaEffects>> magmaEffects_;
};

