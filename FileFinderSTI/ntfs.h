#pragma once

#pragma pack(push,1)
struct BootSector {
   uint8_t     jump[3];
   char        name[8];
   uint16_t    bytesPerSector;
   uint8_t     sectorsPerCluster;
   uint16_t    reservedSectors;
   uint8_t     unused0[3];
   uint16_t    unused1;
   uint8_t     media;
   uint16_t    unused2;
   uint16_t    sectorsPerTrack;
   uint16_t    headsPerCylinder;
   uint32_t    hiddenSectors;
   uint32_t    unused3;
   uint32_t    unused4;
   uint64_t    totalSectors;
   uint64_t    mftStart;
   uint64_t    mftMirrorStart;
   uint32_t    clustersPerFileRecord;
   uint32_t    clustersPerIndexBlock;
   uint64_t    serialNumber;
   uint32_t    checksum;
   uint8_t     bootloader[426];
   uint16_t    bootSignature;
};

struct FileRecordHeader {
   uint32_t    magic;
   uint16_t    updateSequenceOffset;
   uint16_t    updateSequenceSize;
   uint64_t    logSequence;
   uint16_t    sequenceNumber;
   uint16_t    hardLinkCount;
   uint16_t    firstAttributeOffset;
   uint16_t    inUse : 1;
   uint16_t    isDirectory : 1;
   uint32_t    usedSize;
   uint32_t    allocatedSize;
   uint64_t    fileReference;
   uint16_t    nextAttributeID;
   uint16_t    unused;
   uint32_t    recordNumber;
};

struct AttributeHeader {
   uint32_t    attributeType;
   uint32_t    length;
   uint8_t     nonResident;
   uint8_t     nameLength;
   uint16_t    nameOffset;
   uint16_t    flags;
   uint16_t    attributeID;
};

struct ResidentAttributeHeader : AttributeHeader {
   uint32_t    attributeLength;
   uint16_t    attributeOffset;
   uint8_t     indexed;
   uint8_t     unused;
};

struct FileNameAttributeHeader : ResidentAttributeHeader {
   uint64_t    parentRecordNumber : 48;
   uint64_t    sequenceNumber : 16;
   uint64_t    creationTime;
   uint64_t    modificationTime;
   uint64_t    metadataModificationTime;
   uint64_t    readTime;
   uint64_t    allocatedSize;
   uint64_t    realSize;
   uint32_t    flags;
   uint32_t    reparse;
   uint8_t     fileNameLength;
   uint8_t     namespaceType;
   wchar_t     fileName[1];
};

struct NonResidentAttributeHeader : AttributeHeader {
   uint64_t    firstCluster;
   uint64_t    lastCluster;
   uint16_t    dataRunsOffset;
   uint16_t    compressionUnit;
   uint32_t    unused;
   uint64_t    attributeAllocated;
   uint64_t    attributeSize;
   uint64_t    streamDataSize;
};

struct RunHeader {
   uint8_t     lengthFieldBytes : 4;
   uint8_t     offsetFieldBytes : 4;
};
#pragma pack(pop)

