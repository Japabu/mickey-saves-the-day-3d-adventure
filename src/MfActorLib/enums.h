#pragma once
// Enumerations of the actor library (values from the SteamboatApp PDB).
enum MfFileMode { MF_FILE_READ = 0, MF_FILE_WRITE = 1 };
enum MfHTErrorCode { MFHT_EC_NO_ERROR = 0, MFHT_EC_ALREADY_IN_TABLE = 1, MFHT_EC_NOT_IN_TABLE = 2 };
enum MfSkillFormat {
    MF_SKILL_FILE_KEYFRAMES = 'KlkS',
    MF_SKILL_FILE_SAMPLES = 'SlkS',
    MF_SKILL_NO_SAVE = 'NlkS'
};
enum MfStreamError {
    MF_STREAM_OK = 0,
    MF_STREAM_CREATE_FAILED = 1,
    MF_STREAM_CORRUPT = 2,
    MF_STREAM_READ_FAILED = 3,
    MF_STREAM_WRITE_FAILED = 4,
    MF_STREAM_SEEK_FAILED = 5
};
// How MfTriangleSet's triangles are stored (names invented: list, strip, fan
// from the vertex counts Create gives them; a new set is EMPTY).
enum MfTriangleSetType {
    MF_TRIANGLE_LIST = 0,       // mNumTriangles index triples
    MF_TRIANGLE_STRIP = 1,      // mNumTriangles + 2 vertices
    MF_TRIANGLE_FAN = 2,        // mNumTriangles + 2 vertices
    MF_LINE_STRIP = 3,          // vertices only (lines, curves)
    MF_TRIANGLE_EMPTY = 4
};
