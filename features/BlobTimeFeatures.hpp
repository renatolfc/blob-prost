/****************************************************************************************
** Implementation of a variation of BASS Features, which has features to encode the
**  relative position between tiles.
**
** REMARKS: - This implementation is basically Erik Talvitie's implementation, presented
**            in the AAAI'15 LGCVG Workshop.
**
** Author: Marlos C. Machado
***************************************************************************************/

#ifndef FEATURES_H
#define FEATURES_H
#include "Features.hpp"
#endif
#ifndef BACKGROUND_H
#define BACKGROUND_H
#include "Background.hpp"
#endif

#include <tuple>
#include <set>
#include <unordered_map>
//#include <sparsehash/unordered_map>
//using google::unordered_map;

struct Disjoint_Set_Element{
    int rowUp, rowDown, columnLeft, columnRight;
    int size;
    int parent;
    int color;
};

using namespace std;

template <typename T>
class ShiftRegister {
    size_t size=5, toRead, toWrite;
    vector<T> registry;

public:
    ShiftRegister() : toRead(0), toWrite(0) {
        registry.resize(size);
    };
    inline T shift(T new_entry) {
        T ret(registry[toRead]);
        registry[toWrite] = new_entry;
        toRead = (toRead + 1) % size;
        toWrite = (toWrite + 1) % size;
        return ret;
    }
};

class BlobTimeFeatures : public Features::Features {
    private:
        Parameters *param;
        Background *background;
        ShiftRegister<vector<vector<tuple<int,int>>>> blobRegister;
        ShiftRegister<vector<int>> colorRegister;

        long long numBasicFeatures, numRelativeFeatures, numTimeDimensionalOffsets, numThreePointOffsets;
        int numColors;
        int colorMultiplier;

        int numResolutions;
        vector<vector<vector<unsigned short> > >* fullNeighbors;
        vector<vector<vector<unsigned short> > >* extraNeighbors;

        vector<vector<tuple<int,int> > > blobs;
        vector<vector<tuple<int,int> > > previousBlobs;
        vector<int> blobActiveColors;
        vector<int> previousBlobActiveColors;

        vector<tuple<int,int> > resolutions;
        vector<tuple<int,int> > numBlocks;
        vector<tuple<int,int> > numOffsets;

        vector<vector<vector<bool> > > bproExistence;
        vector<vector<tuple<int,int> > > changed;

        vector<unordered_map<long long,int> > threePointExistence;

        vector<long long> baseBpro, baseTime, baseBasic, baseThreePoint;

        bool fakeAle;
        int frameSkip;
        int neighborSize;

    void getBlobs(const ALEScreen &screen);
    void getBasicFeatures(vector<long long>& features);
    void addRelativeFeaturesIndices(vector<long long>& features);
    void addTimeDimensionalOffsets(vector<long long>& features);
    void addThreePointOffsetsIndices(vector<long long>& features, tuple<int,int>& offset, tuple<int,int>& p1, long long& bproIndex);
    void resetBproExistence();
    void resetThreePointExistence();
    void updateRepresentatiePixel(int& x, int& y, Disjoint_Set_Element* root, Disjoint_Set_Element* other);
    int getPowerTwoOffset(int rawDelta);

    public:
        /**
        * Destructor, used to delete the background, which is allocated dynamically.
        */
        ~BlobTimeFeatures();
        /**
         * TODO: COMMENT
         *
         * @param Parameters *param, which gives access to the number of columns, number of rows,
         *                   number of colors and the background information
         * @return nothing, it is a constructor.
         */
        BlobTimeFeatures(Parameters *param);
        /**
         * This method is the instantiation of the virtual method in the class Features (also check
         * its documentation). It iterates over all tiles defined by the columns and rows and checks
         * if each of the colors to be evaluated are present in the tile.
         *
         * REMARKS: - It is necessary to provide both the screen and the ram because of the superclass,
         * despite the RAM being useless here. In fact a null pointer works just fine.
         *          - To avoid return huge vectors, this method is void and the appropriate
         * vector is returned trough a parameter passed by reference.
         *          - This method was adapted from Sriram Srinivasan's code
         *
         * @param ALEScreen &screen is the current game screen that one may use to extract features.
         * @param ALERAM &ram is the current game RAM that one may use to extract features.
         * @param vector<int>& features an empy vector that will be filled with the requested information,
         *        therefore it must be passed by reference. It contain the active indices.
         * @return nothing as one will receive the requested data by the last parameter, by reference.
         */
        void getActiveFeaturesIndices(const ALEScreen &screen, const ALERAM &ram, vector<long long>& features);
        /**
         * Obtain the total number of features that are generated by this feature representation.
         * Since the constructor demands the number of colors, rows and columns to be set, ideally this
         * method will always return a correct number. For this representation it is only the product
         * between these three quantities.
         * Using Bellemare et. al approach, cited above, the total number of features is 28,672.
         *
         * @param none.
         * @return int number of features generated by this method.
         */
        long long getNumberOfFeatures();
        void clearCash();
};
