/*
 * This file is part of crunch
 * Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * crunch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * crunch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#include <stdio.h>
#include "Core.h"
#include "Logger.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Memory.h"
#include <dlfcn.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static const int ok = 0;
const arg args[] =
{
	{"--log", 1, 1, 0},
	{0}
};

parsedArg **parsedArgs = NULL;
parsedArg **namedTests = NULL;
uint32_t numTests = 0;
const char *cwd = NULL;
uint8_t loggingTests = 0;

typedef void (__cdecl *registerFn)();

void *testRunner(void *self)
{
	unitTest *test = (unitTest *)self;
	if (isTTY != 0)
		testPrintf(INFO);
	testPrintf("%s...", test->theTest->testName);
	if (isTTY != 0)
		testPrintf(NEWLINE);
	else
		testPrintf(" ");
	test->theTest->testFunc();
	// Did the test switch logging on?
	if (loggingTests == 0 && logging == 1)
		// Yes, switch it back off again
		stopLogging(logger);
	logResult(RESULT_SUCCESS, "");
	pthreadExit(&ok);
}

void printStats()
{
	uint64_t total = passes + failures;
	testPrintf("Total tests: %zu,  Failures: %u,  Pass rate: ", total, failures);
	if (total == 0)
		testPrintf("--\n");
	else
		testPrintf("%0.2f%%\n", ((double)passes) / ((double)total) * 100.0);
}

uint8_t getTests()
{
	uint32_t i, j, n;
	for (n = 0; parsedArgs[n] != NULL; n++);
	namedTests = testMalloc(sizeof(parsedArg *) * (n + 1));

	for (j = 0, i = 0; i < n; i++)
	{
		if (findArgInArgs(parsedArgs[i]->value) == NULL)
		{
			namedTests[j] = parsedArgs[i];
			j++;
		}
	}
	if (j == 0)
	{
		free(namedTests);
		return FALSE;
	}
	else
	{
		namedTests = testRealloc(namedTests, sizeof(parsedArg *) * (j + 1));
		numTests = j;
		return TRUE;
	}
}

uint8_t tryRegistration(void *testSuit)
{
	registerFn registerTests;
	registerTests = dlsym(testSuit, "registerTests");
	if (registerTests == NULL)
	{
		dlclose(testSuit);
		return FALSE;
	}
	registerTests();
	return TRUE;
}

void runTests()
{
	pthread_attr_t threadAttrs;
	uint32_t i;
	test *currTest;
	log *logFile = NULL;

	parsedArg *logging = findArg(parsedArgs, "--log", NULL);
	if (logging != NULL)
	{
		logFile = startLogging(logging->params[0]);
		loggingTests = 1;
	}
	pthread_attr_init(&threadAttrs);
	pthread_attr_setdetachstate(&threadAttrs, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&threadAttrs, PTHREAD_SCOPE_PROCESS);

	for (i = 0; i < numTests; i++)
	{
		char *testLib = formatString("%s/%s.so", cwd, namedTests[i]->value);
		void *testSuit = dlopen(testLib, RTLD_LAZY);
		free(testLib);
		if (testSuit == NULL || tryRegistration(testSuit) == FALSE)
		{
			if (testSuit == NULL)
			{
				if (isTTY != 0)
					testPrintf(FAILURE);
				printf("Could not open test library: %s", dlerror());
				if (isTTY != 0)
					printf(NEWLINE);
				else
					printf("\n");
			}
			if (isTTY != 0)
				testPrintf(FAILURE);
			testPrintf("Test library %s was not a valid library, skipping", namedTests[i]->value);
			if (isTTY != 0)
				testPrintf(NEWLINE);
			else
				testPrintf("\n");
			continue;
		}
		if (isTTY != 0)
			testPrintf(COLOUR("1;35"));
		testPrintf("Running test suit %s...", namedTests[i]->value);
		if (isTTY != 0)
			testPrintf(NEWLINE);
		else
			testPrintf("\n");
		currTest = tests;
		while (currTest->testFunc != NULL)
		{
			int *retVal;
			unitTest *test = testMalloc(sizeof(unitTest));
			test->theTest = currTest;
			test->testThread = testMalloc(sizeof(pthread_t));
			pthread_create(test->testThread, &threadAttrs, testRunner, test);
			pthread_join(*test->testThread, (void **)&retVal);
			if (retVal == NULL || *retVal == 2)
				exit(2);
			currTest++;
		}
	}

	pthread_attr_destroy(&threadAttrs);
	printStats();
	if (logging != NULL)
		stopLogging(logFile);
}

int main(int argc, char **argv)
{
	parsedArgs = parseArguments(argc, argv);
	if (parsedArgs == NULL || getTests() == FALSE)
	{
		testPrintf("Fatal error: There are no tests to run given on the command line!\n");
		return 2;
	}
	cwd = getcwd(NULL, 0);
	isTTY = isatty(STDOUT_FILENO);
#ifdef _MSC_VER
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console == NULL)
	{
		printf("Error: could not grab console!");
		exit(1);
	}
#endif
	runTests();
	free((void *)cwd);
	return 0;
}
