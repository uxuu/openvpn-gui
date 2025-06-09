/**
* @file openvpn-ex.h
 * @brief Header file for OpenVPN C++ extended functionalities.
 * @details This file provides declarations for extended features and utilities related to the OpenVPN functionality,
 * such as enhanced connection management, and utility functions for runtime operations.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-06-08
 */

#ifndef OPENVPN_EX_H
#define OPENVPN_EX_H

#ifdef __cplusplus
extern "C" {
#endif

    void InitStatusPage(connection_t* c);
    void ReleaseStatusPage(connection_t* c);
    void ShowStatusPage(connection_t* c, BOOL bShow);
#ifdef __cplusplus
    class TaskSingleton {
    public:
        static TaskSingleton *getInstance();
    public:
        void InitStatusPage(connection_t* c);
        void ReleaseStatusPage(connection_t* c);
        void ShowStatusPage(connection_t* c, BOOL bShow);
        int GetStatusPageIndex(connection_t *c, STabCtrlEx *pTab = NULL);
        STabPage *GetStatusPage(connection_t *c, STabCtrlEx *pTab = NULL);
    private:
        TaskSingleton() {};
        TaskSingleton(TaskSingleton&) = delete;
    };
#endif

#ifdef __cplusplus
}
#endif

#endif //OPENVPN_EX_H
