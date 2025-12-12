#pragma once
#include "AnimationData.h"

class AnimationController {

private:
	AnimationData* _animationData;
	Matrix boneGlobalPoseMatrices[256];
	std::vector<int> boneOrder;
	std::string currentAnimationName;
	float t;
	Matrix coordTransform;

public:

	Matrix skinningMatrices[256];

	void init(AnimationData* animationData, int fromYZX = 0) {
		_animationData = animationData; 
		buildBoneOrder();
	}

	void resetAnimationTime() {
		t = 0;
	}

	bool animationFinished() {
		if (!_animationData) return true;
		AnimationSequence seq = _animationData->getAnimationSequence(currentAnimationName);
		return t >= seq.duration();
	}

	void update(std::string name, float dt) {

		// Update animation time
		if (name == currentAnimationName) {
			t += dt;
		}
		else {
			currentAnimationName = name;
			t = 0;
		}

		if (animationFinished() == true) {
			return;
		}

		// Get frame index and interpolation weight
		int frameIndex = 0;
		float interpolationWeight = 0;
		_animationData->getFrameIndexAndInterpolationWeight(name, t, frameIndex, interpolationWeight);

		// FK to get bone global pose matrices
		for (int boneIdx : boneOrder) {
			boneGlobalPoseMatrices[boneIdx] = 
				_animationData->getInterpolatedBoneMatrices(
					currentAnimationName,
					boneGlobalPoseMatrices,
					frameIndex,
					interpolationWeight,
					boneIdx
				);
		}

		// Skinning matrices
		_animationData->getSkinningMatrices(boneGlobalPoseMatrices, skinningMatrices, coordTransform);
	}

private:
	void buildBoneOrder() {
		boneOrder.clear();
		if (!_animationData) return;

		int n = _animationData->bonesSize();
		if (n <= 0) return;

		for (int i = 0; i < n; ++i) {
			if (_animationData->getBoneParentByIndex(i) == -1) {
				dfsAddBone(i, n);
			}
		}
	}

	void dfsAddBone(int boneIndex, int boneCount) {
		boneOrder.push_back(boneIndex);
		for (int i = 0; i < boneCount; ++i) {
			if (_animationData->getBoneParentByIndex(i) == boneIndex) {
				dfsAddBone(i, boneCount);
			}
		}
	}
};