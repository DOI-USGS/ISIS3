#include <QTemporaryFile>
#include <QString>
#include <iostream>

#include "Brick.h"
#include "Cube.h"
#include "CubeTileHandler.h"
#include "SpecialPixel.h"

#include "TempFixtures.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

class ReadWriteCube : public TempTestingFiles {
protected:
    Cube *cube = new Cube;
    Brick *localBrick = NULL;
    QString path;

    void SetUp() {
        TempTestingFiles::SetUp();
        
        path = tempDir.path() + "/tiny.cub";
        cube->close();
    }

    void TearDown() {
        if (cube) {
            delete cube;
        }
        if (localBrick) {
            delete localBrick;
        }
    }

    void createSizedCube(int samples, int lines, int bands, PixelType pixelType) {
        cube->setDimensions(samples, lines, bands);
        cube->setPixelType(pixelType);
        cube->setBaseMultiplier(0.0, 1.0);
        cube->create(path);
        cube->close();
    }

    void createCube(PixelType pixelType, bool write=true) {
        createSizedCube(6, 1, 1, pixelType);
        if (write) {
            cube->open(path, "rw");

            localBrick = new Brick(6, 1, 1, pixelType);
            localBrick->SetBasePosition(1, 1, 1);
            double *brickDoubleBuff = localBrick->DoubleBuffer();
            brickDoubleBuff[0] = HIGH_INSTR_SAT8;
            brickDoubleBuff[1] = HIGH_REPR_SAT8;
            brickDoubleBuff[2] = LOW_INSTR_SAT8;
            brickDoubleBuff[3] = LOW_REPR_SAT8;
            brickDoubleBuff[4] = NULL8;
            if (pixelType == Double) {
                brickDoubleBuff[5] = 1000;
            }
            else if (pixelType == Real) {
                brickDoubleBuff[5] = 1000;
            }
            else if (pixelType == SignedInteger) {
                brickDoubleBuff[5] = 1000;
            }
            else if (pixelType == UnsignedInteger) {
                brickDoubleBuff[5] = 1000;
            }
            else if (pixelType == SignedWord) {
                brickDoubleBuff[5] = 1000;
            }
            else if (pixelType == UnsignedWord) {
                brickDoubleBuff[5] = 1000;
            }
            else if (pixelType == SignedByte) {
                brickDoubleBuff[5] = 50;
            }
            else if (pixelType == UnsignedByte) {
                brickDoubleBuff[5] = 50;
            }

            cube->write(*localBrick);
            cube->close();
            delete localBrick;
        }
    }
};

class IsisDNTypeGenerator: public ReadWriteCube, public ::testing::WithParamInterface<Isis::PixelType> {
    // Intentionally left empty
    void SetUp() {
        TempTestingFiles::SetUp();
        ReadWriteCube::SetUp();
    }

    void TearDown() {
        TempTestingFiles::TearDown();
        ReadWriteCube::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P (IsisDNPixelTypes,
                          IsisDNTypeGenerator,
                          ::testing::Values(Isis::UnsignedByte,
                                            // Isis::SignedByte,      // Unsupported
                                            Isis::UnsignedWord,
                                            Isis::SignedWord,
                                            Isis::UnsignedInteger,
                                            // Isis::SignedInteger,   // Unsupported
                                            Isis::Real,
                                            Isis::Double));

TEST_P(IsisDNTypeGenerator, CubeIoTestsReadWrite) {
    PixelType pixelType = GetParam();
    createCube(pixelType, false);

    localBrick = new Brick(6, 1, 1, pixelType);
    localBrick->SetBasePosition(1, 1, 1);
    double *brickDoubleBuff = localBrick->DoubleBuffer();
    brickDoubleBuff[0] = HIGH_INSTR_SAT8;
    brickDoubleBuff[1] = HIGH_REPR_SAT8;
    brickDoubleBuff[2] = LOW_INSTR_SAT8;
    brickDoubleBuff[3] = LOW_REPR_SAT8;
    brickDoubleBuff[4] = NULL8;
    brickDoubleBuff[5] = 50.0;

    QList bands = {1};
    cube->open(path);
    Pvl label = *cube->label();
    cube->close();
    QFile file(path);
    if (!file.open(QIODevice::ReadWrite)) {
        QString msg = "Failed to open [" + path + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    CubeTileHandler handler(&file, &bands, label, true);
    handler.write(*localBrick);
    delete localBrick;

    localBrick = new Brick(6, 1, 1, pixelType);
    localBrick->SetBasePosition(1, 1, 1);
    handler.read(*localBrick);

    brickDoubleBuff = localBrick->DoubleBuffer();
    if (pixelType == UnsignedByte || pixelType == SignedByte) {
        EXPECT_EQ(brickDoubleBuff[0], HIGH_REPR_SAT8);
        EXPECT_EQ(brickDoubleBuff[1], HIGH_REPR_SAT8);
        EXPECT_EQ(brickDoubleBuff[2], NULL8);
        EXPECT_EQ(brickDoubleBuff[3], NULL8);
        EXPECT_EQ(brickDoubleBuff[4], NULL8);
    }
    else if (pixelType == UnsignedWord) {
        EXPECT_EQ(brickDoubleBuff[0], 65534);
        EXPECT_EQ(brickDoubleBuff[1], 65535);
        EXPECT_EQ(brickDoubleBuff[2], LOW_INSTR_SAT8);
        EXPECT_EQ(brickDoubleBuff[3], LOW_REPR_SAT8);
        EXPECT_EQ(brickDoubleBuff[4], NULL8);
    }
    else if (pixelType == UnsignedInteger) {
        EXPECT_EQ(brickDoubleBuff[0], 4294967294);
        EXPECT_EQ(brickDoubleBuff[1], 4294967295);
        EXPECT_EQ(brickDoubleBuff[2], LOW_INSTR_SAT8);
        EXPECT_EQ(brickDoubleBuff[3], LOW_REPR_SAT8);
        EXPECT_EQ(brickDoubleBuff[4], NULL8);
    }
    else {
        EXPECT_EQ(brickDoubleBuff[0], HIGH_INSTR_SAT8);
        EXPECT_EQ(brickDoubleBuff[1], HIGH_REPR_SAT8);
        EXPECT_EQ(brickDoubleBuff[2], LOW_INSTR_SAT8);
        EXPECT_EQ(brickDoubleBuff[3], LOW_REPR_SAT8);
        EXPECT_EQ(brickDoubleBuff[4], NULL8);
    }
    EXPECT_EQ(brickDoubleBuff[5], 50.0);

    void *rawBuffer = localBrick->RawBuffer();
    if (pixelType == Double) {
        ((double *)rawBuffer)[0] = HIGH_INSTR_SAT8;
        ((double *)rawBuffer)[1] = HIGH_REPR_SAT8;
        ((double *)rawBuffer)[2] = LOW_INSTR_SAT8;
        ((double *)rawBuffer)[3] = LOW_REPR_SAT8;
        ((double *)rawBuffer)[4] = NULL8;
        ((double *)rawBuffer)[5] = 50;
    }
    else if (pixelType == Real) {
        ((float *)rawBuffer)[0] = HIGH_INSTR_SAT4;
        ((float *)rawBuffer)[1] = HIGH_REPR_SAT4;
        ((float *)rawBuffer)[2] = LOW_INSTR_SAT4;
        ((float *)rawBuffer)[3] = LOW_REPR_SAT4;
        ((float *)rawBuffer)[4] = NULL4;
        ((float *)rawBuffer)[5] = 50;
    }
    else if (pixelType == UnsignedInteger) {
        ((unsigned int *)rawBuffer)[0] = HIGH_INSTR_SATUI4;
        ((unsigned int *)rawBuffer)[1] = HIGH_REPR_SATUI4;
        ((unsigned int *)rawBuffer)[2] = LOW_INSTR_SATUI4;
        ((unsigned int *)rawBuffer)[3] = LOW_REPR_SATUI4;
        ((unsigned int *)rawBuffer)[4] = NULLUI4;
        ((unsigned int *)rawBuffer)[5] = 50;
    }
    else if (pixelType == SignedWord) {
        ((short *)rawBuffer)[0] = HIGH_INSTR_SAT2;
        ((short *)rawBuffer)[1] = HIGH_REPR_SAT2;
        ((short *)rawBuffer)[2] = LOW_INSTR_SAT2;
        ((short *)rawBuffer)[3] = LOW_REPR_SAT2;
        ((short *)rawBuffer)[4] = NULL2;
        ((short *)rawBuffer)[5] = 50;
    }
    else if (pixelType == UnsignedWord) {
        ((unsigned short *)rawBuffer)[0] = HIGH_INSTR_SATU2;
        ((unsigned short *)rawBuffer)[1] = HIGH_REPR_SATU2;
        ((unsigned short *)rawBuffer)[2] = LOW_INSTR_SATU2;
        ((unsigned short *)rawBuffer)[3] = LOW_REPR_SATU2;
        ((unsigned short *)rawBuffer)[4] = NULLU2;
        ((unsigned short *)rawBuffer)[5] = 50;
    }
    else if (pixelType == UnsignedByte) {
        ((unsigned char *)rawBuffer)[0] = HIGH_INSTR_SAT1;
        ((unsigned char *)rawBuffer)[1] = HIGH_REPR_SAT1;
        ((unsigned char *)rawBuffer)[2] = LOW_INSTR_SAT1;
        ((unsigned char *)rawBuffer)[3] = LOW_REPR_SAT1;
        ((unsigned char *)rawBuffer)[4] = NULL1;
        ((unsigned char *)rawBuffer)[5] = 50;
    }
}
