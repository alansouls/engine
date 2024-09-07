#include "../engine/scenes/GameObject.h"
#include <chrono>

class Racket : public GameObject
{
public:
	Racket(bool left);

	void init() override;

	void update() override;

private:
	bool m_left;
	std::chrono::high_resolution_clock::time_point m_lastTime;

	float m_originalWindowWidth;
	float m_originalWindowHeight;

	float m_lastWindowWidth;
	float m_lastWindowHeight;

	float m_width;
	float m_height;

	float m_steps[2] = { -1.0f, 1.0f };

	const float m_topLimit = 5.0f;
	float m_bottomLimit;

	uint8_t m_currentStep;
};