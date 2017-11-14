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
#define FIELD_SIZE 7	

#define NUMWORD_CHECK 0
#define TITLE_CHECK 1
#define MIN_CHECK 2
#define MAX_CHECK 3
#define CSR_CHECK 4
#define BITS_CHECK 5
#define COMMENT_CHECK 6

// ������� DLL � �����������
#define LOG_THREAD_BUSY L"����� �����! ��������� ��������� ��������!"
#define LOG_SLASH	L"================================================================\n"
#define LOG_START	L"���������� ��������"
#define LOD_END		L"���������� �������"
#define LOG_OPEN	L"�������� ��� ���������"
#define LOG_REFRESH L"���������� ��� ���������"
#define LOG_ADD		L"���������� ��� ���������"
#define LOG_CLOSE	L"�������� ��� ���������"
#define LOG_FOLDER	L"�������� ������������ ����� �������"
#define LOG_ERROR	L"��� �������� ��������� ������ (��������� �������� ��. � �����)"