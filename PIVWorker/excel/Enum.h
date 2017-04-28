#pragma once

typedef enum  XlFindLookIn
{
	xlFormulas = -4123,
	xlComments = -4144,
	xlValues = -4163
};

typedef enum  XlLookAt
{
	xlPart = 2,
	xlWhole = 1
};

typedef enum  XlSearchDirection
{
	xlNext = 1,
	xlPrevious = 2
};

typedef enum  XlSearchOrder
{
	xlByColumns = 2,
	xlByRows = 1
};
