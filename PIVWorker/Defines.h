#pragma once

// ����������� ������� ��� �������� DLL

#ifdef PIV_WORKER_EXPORTS
#define PIV_DECLARE __declspec(dllimport)
#else
#define PIV_DECLARE __declspec(dllexport)
#endif

// �����
#define BASE_FOLDER L"\\Artefacts"
#define TEXT_FOLDER L"\\Text"
#define PROJECT_FOLDER L"\\Project"
#define OTHER_FOLDER L"\\Other"
#define ERROR_FOLDER L"\\Error"
#define SYNTAX_FOLDER L"\\Error\\Syntax"
#define SIMANTIC_FOLDER L"\\Error\\Simantic"
#define WARNING_FOLDER L"\\Warning"

// ����� �������� �������� �����
#define PROJECT_SIZE 8
#define OTHER_SIZE 6

// �����
#define REPORT_NAME L"�����.html"
#define LOG_FILE_NAME L"log.txt"

// �������������� �������� �������
#define RESERVE_SIGNAL L"������"