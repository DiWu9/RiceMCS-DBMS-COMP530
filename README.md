# RiceMCS-DBMS-COMP530

Build a database management system using c++ from the ground up.

## Overview

The entire database system has the follow layers (the lower the more basic):

- DML parser / compiler
- Logical Optimizer
- Physical Optimizer
- Query Executer Engine
- Index/File/Rec manager
- Buffer Manager
- Persistant Store

## A1: Buffer Manager

Description: The buffer manager accerlerate the page lookup by buffering pages in the buffer pool. The buffer pool has LRU eviction policy.
