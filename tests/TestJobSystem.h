#pragma once
#include <gtest/gtest.h>
#include <memory>
#include <functional>
#include <future>
#include "Engine/JobSystem.h"

class JobSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialization code here
        jobSystem = JobSystem::CreateJobSystem();
    }

    void TearDown() override {
        // Clean-up code here
        JobSystem::DestroyJobSystem(jobSystem);
    }

    JobSystem* jobSystem = nullptr;
};

TEST_F(JobSystemTest, JobExecution) {
    bool executed = false;
    auto job = std::make_unique<Job>([&executed]() { executed = true; });
    jobSystem->SubmitJob(job);
    // Assuming there's a way to ensure the job has been executed, like a flush or sync method in JobSystem
    // jobSystem->FlushJobs();
    EXPECT_TRUE(executed);
}

TEST_F(JobSystemTest, JobPriority) {
    auto jobHighPriority = std::make_unique<Job>([]() {}, 0, 10); // Higher priority
    auto jobLowPriority = std::make_unique<Job>([]() {}, 0, 1); // Lower priority

    EXPECT_LT(jobLowPriority->GetPriority(), jobHighPriority->GetPriority());
}

TEST_F(JobSystemTest, JobIsFinished) {
    auto job = std::make_unique<Job>([]() {});
    job->Execute();
    EXPECT_TRUE(job->IsFinished());
}

int AddNumber(int a, int b)
{ return a + b; }

