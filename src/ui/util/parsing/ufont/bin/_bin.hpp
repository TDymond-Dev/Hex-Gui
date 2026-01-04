#ifndef TRUETYPE5
#define TRUETYPE5

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace detail {
	std::string u32_to_str(uint32_t u32) {
		uint8_t p1 = (uint32_t)(u32), p2 = (uint32_t)(u32 >> 8), p3 = (uint32_t)(u32 >> 16), p4 = (uint32_t)(u32 >> 24);
		return std::string{ (char)p4, (char)p3, (char)p2, (char)p1 };
	}

	struct Parser {
		Parser(std::string const& path) : currenntIndex{ 0 } {
			std::ifstream stream(path.c_str(), std::ios::binary);
			std::streampos fsize = stream.tellg();

			stream.seekg(0, std::ios::end);
			fsize = stream.tellg() - fsize;
			stream.seekg(0);

			buffer = std::vector<unsigned char>(fsize, unsigned char{});
			stream.read(reinterpret_cast<char*>(buffer.data()), fsize);
			size = buffer.size();
		}

		template<typename T> auto vec(size_t s) { return std::vector<T>(s, T{}); }
		template<typename K, typename V> auto umap(size_t s) { return std::unordered_map<K, V>(); }

		template<typename T> void arr(std::vector<T>& t) {
			memcpy(&t[0], &buffer[currenntIndex], t.size());
			currenntIndex += t.size();
		}

		uint8_t u8() { return (uint8_t)buffer[currenntIndex++]; }
		uint16_t u16() { return (u8() << 8) | (u8() << 0); }
		uint32_t u32() { return (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }
		uint64_t u64() { return (u8() << 56) | (u8() << 48) | (u8() << 40) | (u8() << 32) | (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }

		int8_t i8() { return u8(); }
		int16_t i16() { return (u8() << 8) | (u8() << 0); }
		int32_t i32() { return (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }
		int64_t i64() { return (u8() << 56) | (u8() << 48) | (u8() << 40) | (u8() << 32) | (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }

		Parser& operator +(uint8_t& v) { v = u8(); return *this; }
		Parser& operator +(uint16_t& v) { v = u16(); return *this; }
		Parser& operator +(uint32_t& v) { v = u32(); return *this; }
		Parser& operator +(uint64_t& v) { v = u64(); return *this; }
		Parser& operator +(int8_t& v) { v = i8(); return *this; }
		Parser& operator +(int16_t& v) { v = i16(); return *this; }
		Parser& operator +(int32_t& v) { v = i32(); return *this; }
		Parser& operator +(int64_t& v) { v = i64(); return *this; }

		size_t rd_position() { return currenntIndex; }
		void set_position(size_t index) { currenntIndex = index; }
	private:
		std::vector<unsigned char> buffer;
		size_t currenntIndex, size;
	};

	struct tbl_directory {
		struct record {
			uint32_t checksum;
			uint32_t offset;
			uint32_t length;
		};
		uint32_t sfntVersion; // 0x00010000 or 0x4F54544F ('OTTO') — see below.
		uint16_t numTables; // Number of tables.
		uint16_t searchRange; // Maximum power of 2 less than or equal to numTables, times 16 ((2**floor(log2(numTables))) * 16, where “**” is an exponentiation operator).
		uint16_t entrySelector; // Log2 of the maximum power of 2 less than or equal to numTables (log2(searchRange/16), which is equal to floor(log2(numTables))).
		uint16_t rangeShift; // numTables times 16, minus searchRange ((numTables * 16) - searchRange)

		//std::vector<record> tableRecords; // Table records array—one for each top-level table in the font
		std::unordered_map<std::string, record> tableRecords;
		void parse(Parser& p, size_t offset = 0) {
			p + sfntVersion + numTables + searchRange + entrySelector + rangeShift;

			tableRecords = p.umap<std::string, record>(numTables);
			for (int i = 0; i < numTables; i++) {
				uint32_t tag = p.u32(), checksum = p.u32(), offset = p.u32(), length = p.u32();
				tableRecords.try_emplace(u32_to_str(tag), record{ checksum, offset, length });
			}
		}
	};

	struct tbl_head {
		uint16_t major_version; // 0x00010000 if (version 1.0)
		uint16_t minor_version; // 0x00010000 if (version 1.0)
		uint32_t fontRevision; // set by font manufacturer
		uint32_t checkSumAdjusted; // To compute: set it to 0, calculate the checksum for the 'head' table and put it in the table directory, sum the entire font as a uint32_t, then store 0xB1B0AFBA - sum. (The checksum for the 'head' table will be wrong as a result. That is OK; do not reset it.)
		uint32_t magicNumber;
		uint16_t flags;
		uint16_t unitsPerEm;
		uint64_t created;
		uint64_t modified;
		int16_t xMin;
		int16_t yMin;
		int16_t xMax;
		int16_t yMax;
		uint16_t macStyle;
		uint16_t lowestRecPPEM;
		int16_t fontDirectionHint;
		int16_t indexToLocFormat;
		int16_t glyphDataFormat;

		void parse(Parser& p, uint32_t offset = 0) {
			p.set_position(offset);

			major_version = p.u16();
			minor_version = p.u16();
			fontRevision = p.u32();
			checkSumAdjusted = p.u32();
			magicNumber = p.u32();
			flags = p.u16();
			unitsPerEm = p.u16();
			created = p.u64();
			modified = p.u64();
			xMin = p.i16();
			yMin = p.i16();
			xMax = p.i16();
			yMax = p.i16();
			macStyle = p.u16();
			lowestRecPPEM = p.u16();
			fontDirectionHint = p.i16();
			indexToLocFormat = p.i16();
			glyphDataFormat = p.i16();
		}
	};

	struct tbl_name {
		struct name_record {
			uint16_t platformID;
			uint16_t platformSpecificID;
			uint16_t languageID;
			uint16_t nameID;
			uint16_t length;
			uint16_t offset;
		};

		uint16_t format;
		uint16_t count;
		uint16_t stringOffset;

		std::vector<name_record> nameRecords;
		std::vector<std::string> strings;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			format = p.u16();
			count = p.u16();
			stringOffset = p.u16();
			nameRecords = p.vec<name_record>(count);
			for (auto& r : nameRecords) {
				p + r.platformID + r.platformSpecificID + r.languageID + r.nameID + r.length + r.offset;
			}
			for (int i = 0; i < count; i++) {
				strings.push_back(std::string(nameRecords[i].length, ' '));
				for (int j = 0; j < nameRecords[i].length - 1; j++)
					strings.back()[j] = p.u8();
			}
		}
	};

	struct tbl_os2 {
		uint16_t version;
		int16_t	xAvgCharWidth;
		uint16_t usWeightClass;
		uint16_t usWidthClass;
		uint16_t fsType;
		int16_t	ySubscriptXSize;
		int16_t	ySubscriptYSize;
		int16_t	ySubscriptXOffset;
		int16_t	ySubscriptYOffset;
		int16_t	ySuperscriptXSize;
		int16_t	ySuperscriptYSize;
		int16_t	ySuperscriptXOffset;
		int16_t	ySuperscriptYOffset;
		int16_t	yStrikeoutSize;
		int16_t	yStrikeoutPosition;
		int16_t	sFamilyClass;
		uint8_t	panose[10];
		uint32_t ulUnicodeRange1;// Bits 0–31
		uint32_t ulUnicodeRange2;	// Bits 32–63
		uint32_t ulUnicodeRange3; //	Bits 64–95
		uint32_t ulUnicodeRange4; //	Bits 96–127
		uint8_t achVendID[4]; // Tag(4 8 bit ints)
		uint16_t fsSelection;
		uint16_t usFirstCharIndex;
		uint16_t usLastCharIndex;
		int16_t	sTypoAscender;
		int16_t	sTypoDescender;
		int16_t	sTypoLineGap;
		uint16_t usWinAscent;
		uint16_t usWinDescent;
		uint32_t ulCodePageRange1;//	Bits 0–31
		uint32_t ulCodePageRange2;//	Bits 32–63
		int16_t	sxHeight;
		int16_t	sCapHeight;
		uint16_t usDefaultChar;
		uint16_t usBreakChar;
		uint16_t usMaxContext;
		uint16_t usLowerOpticalPointSize;
		uint16_t usUpperOpticalPointSize;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			p + version + xAvgCharWidth + usWeightClass + usWidthClass + fsType +
				ySubscriptXSize + ySubscriptYSize + ySubscriptXOffset + ySubscriptYOffset + ySuperscriptXSize +
				ySuperscriptYSize + ySuperscriptXOffset + ySuperscriptYOffset + yStrikeoutSize + yStrikeoutPosition +
				sFamilyClass;

			for (int i = 0; i < 10; i++)
				panose[i] = p.u8();

			p + ulUnicodeRange1 + ulUnicodeRange2 + ulUnicodeRange3 + ulUnicodeRange4;

			for (int i = 0; i < 4; i++)
				achVendID[i] = p.u8();

			p + fsSelection + usFirstCharIndex + usLastCharIndex + sTypoAscender + sTypoDescender + sTypoLineGap +
				usWinAscent + usWinDescent + ulCodePageRange1 + ulCodePageRange2 + sxHeight +
				sCapHeight + usDefaultChar + usBreakChar + usMaxContext + usLowerOpticalPointSize +
				usUpperOpticalPointSize;
		}
	};

	struct tbl_cvt {
		std::vector<int16_t> instructionValues;

		void parse(Parser& p, size_t offset, size_t length) {
			p.set_position(offset);

			instructionValues = p.vec<int16_t>(length / sizeof(int16_t));
			for (auto& iv : instructionValues)
				iv = p.i16();
		}
	};

	struct tbl_kern {
		// Format 0 is the only subtable format supported by Windows.
		struct kern_subtable_0 {
			uint16_t nPairs; // This gives the number of kerning pairs in the table.
			uint16_t searchRange;
			uint16_t entrySelector;
			uint16_t rangeShift;

			std::vector<uint16_t> left; // The glyph index for the left-hand glyph in the kerning pair.
			std::vector<uint16_t> right; // The glyph index for the right-hand glyph in the kerning pair.
			std::vector<int32_t> value; // The kerning value for the above pair, in FUnits. If this value is greater than zero, the characters will be moved apart. If this value is less than zero, the character will be moved closer together.
		};

		struct kern_subtable_2 {

		};

		struct kern_header {
			uint16_t version; //	Kern subtable version number
			uint16_t length; //		Length of the subtable, in bytes(including this header).
			uint16_t coverage; //	What type of information is contained in this table.

			uint8_t format; // 0 format 0, 2 format 2
			uint16_t index; // index into the array of tables
		};

		uint16_t version;
		uint16_t nTables;
		std::vector<kern_header> kern_headers;
		std::vector<kern_subtable_0> subtables_0;
		std::vector<kern_subtable_2> subtables_2;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			version = p.u16();
			nTables = p.u16();

			for (int i = 0; i < nTables; i++) {
				kern_header header;
				header.version = p.u16();
				header.length = p.u16();
				header.coverage = p.u16();
				header.format = (uint16_t)(header.coverage >> 8);
				header.index = (header.format == 0) ? subtables_0.size() : subtables_2.size();

				if (header.format == 0) { // subtable 0
					kern_subtable_0 subtable_0;
					subtable_0.nPairs = p.u16();
					subtable_0.searchRange = p.u16();
					subtable_0.entrySelector = p.u16();
					subtable_0.rangeShift = p.u16();

					subtable_0.left = p.vec<uint16_t>(subtable_0.nPairs);
					subtable_0.right = p.vec<uint16_t>(subtable_0.nPairs);
					subtable_0.value = p.vec<int32_t>(subtable_0.nPairs);

					for (int i = 0; i < subtable_0.nPairs; i++) {
						uint32_t sign = p.u32();
						subtable_0.left[i] = (uint16_t)(sign >> 16), subtable_0.right[i] = (uint16_t)(sign >> 0);
						subtable_0.value[i] = p.i16();
					}
				}
			}
		}
	};

	struct tbl_maxp {
		uint32_t	version;	//0x00010000 (1.0) pack (16, 16)
		uint16_t	numGlyphs;	//the number of glyphs in the font
		uint16_t	maxPoints;	// points in non - compound glyph
		uint16_t	maxContours;	// contours in non - compound glyph
		uint16_t	maxComponentPoints;	// points in compound glyph
		uint16_t	maxComponentContours;	// contours in compound glyph
		uint16_t	maxZones; //	set to 2
		uint16_t	maxTwilightPoints;	// points used in Twilight Zone(Z0)
		uint16_t	maxStorage;			// number of Storage Area locations
		uint16_t	maxFunctionDefs;	// number of FDEFs
		uint16_t	maxInstructionDefs;	// number of IDEFs
		uint16_t	maxStackElements;	// maximum stack depth
		uint16_t	maxSizeOfInstructions; //  byte count for glyph instructions
		uint16_t	maxComponentElements; // number of glyphs referenced at top level
		uint16_t	maxComponentDepth; // levels of recursion, set to 0 if font has only simple glyphs

		void parse(Parser& p, uint32_t offset = 0) {
			p.set_position(offset);
			version = p.u32();
			numGlyphs = p.u16();
			maxPoints = p.u16();
			maxContours = p.u16();
			maxComponentPoints = p.u16();
			maxComponentContours = p.u16();
			maxZones = p.u16();
			maxTwilightPoints = p.u16();
			maxStorage = p.u16();
			maxFunctionDefs = p.u16();
			maxInstructionDefs = p.u16();
			maxStackElements = p.u16();
			maxSizeOfInstructions = p.u16();
			maxComponentElements = p.u16();
			maxComponentDepth = p.u16();
		}
	};

	struct tbl_hhea {
		int32_t	version; //	0x00010000 (1.0) (16, 16)
		int16_t	ascent;//	Distance from baseline of highest ascender
		int16_t	descent;//	Distance from baseline of lowest descender
		int16_t	lineGap;//	typographic line gap
		uint16_t advanceWidthMax;//	must be consistent with horizontal metrics
		int16_t	minLeftSideBearing;//	must be consistent with horizontal metrics
		int16_t	minRightSideBearing;//	must be consistent with horizontal metrics
		int16_t	xMaxExtent;//	max(lsb + (xMax - xMin))
		int16_t	caretSlopeRise;//	used to calculate the slope of the caret(rise / run) set to 1 for vertical caret
		int16_t	caretSlopeRun;//	0 for vertical
		int16_t	caretOffset;//	set value to 0 for non - slanted fonts
		int16_t	reserved1;//	set value to 0
		int16_t	reserved2;//	set value to 0
		int16_t	reserved3;//	set value to 0
		int16_t	reserved4;//	set value to 0
		int16_t	metricDataFormat;//	0 for current format
		uint16_t numOfLongHorMetrics;//	number of advance widths in metrics table

		void parse(Parser& p, uint32_t offset = 0) {
			p.set_position(offset);
			version = p.i32();
			ascent = p.i16();
			descent = p.i16();
			lineGap = p.i16();
			advanceWidthMax = p.u16();
			minLeftSideBearing = p.i16();
			minRightSideBearing = p.i16();
			xMaxExtent = p.i16();
			caretSlopeRise = p.i16();
			caretSlopeRun = p.i16();
			caretOffset = p.i16();
			reserved1 = p.i16();
			reserved2 = p.i16();
			reserved3 = p.i16();
			reserved4 = p.i16();
			metricDataFormat = p.i16();
			numOfLongHorMetrics = p.u16();
		}
	};

	struct tbl_hmtx {
		struct hmtx_entry {
			uint16_t advanceWidth;
			int16_t leftSideBearing;
		};
		std::vector<hmtx_entry> entries;

		void parse(uint32_t noOfEntries, Parser& p, uint32_t offset = 0) {
			p.set_position(offset);
			entries = p.vec<hmtx_entry>(noOfEntries);
			for (auto& e : entries)
				e.advanceWidth = p.u16(), e.leftSideBearing = p.i16();
		}
	};

	// The indexToLoc table stores the offsets to the locations of the glyphs in the 
	// font, relative to the beginning of the glyphData table. In order to compute the 
	// length of the last glyph element, there is an extra entry after the last valid index
	struct tbl_loca {
		std::vector<uint32_t> offsets;

		void parse(uint32_t locaFormat, uint32_t size, Parser& p, uint32_t offset = 0) {
			p.set_position(offset);

			offsets = p.vec<uint32_t>(size + 1);
			for (auto& o : offsets)
				o = (locaFormat) ? p.u32() : p.u16() / 2;
		}
	};

	struct tbl_glyf {
		struct Glyph {
			int16_t noOfContours;
			int16_t xMin;
			int16_t yMin;
			int16_t xMax;
			int16_t yMax;

			uint16_t index;

		};

		struct SimpleGlyph {
			struct Flag {
				bool offCurve;
				bool xShort;
				bool yShort;
				bool repeat;
				bool xDual;
				bool yDual;
				bool overlap_deprec;
				bool resevred;
			};


			struct Outline {
				std::vector<uint16_t> endPtsOfContours;
				uint16_t instructionLength;
				std::vector<uint8_t> instructions;
				std::vector<Flag> flags;
				std::vector<int16_t> xCoords;
				std::vector<int16_t> yCoords;
			};
			Outline outline;
		};

		struct CompoundGlyph {

		};

		std::vector<Glyph> glyphs;
		std::vector<SimpleGlyph> simpleGlyphs;
		std::vector<CompoundGlyph> compoundGlyphs;

		void parse(uint32_t size, std::vector<uint32_t> const& offsets, Parser& p, uint32_t offset = 0) {
			p.set_position(offset);

			glyphs = p.vec<Glyph>(size);
			int k = 0, i = 0;
			for (auto& glyph : glyphs) {
				p.set_position(offset + offsets[i++]);

				glyph.noOfContours = p.i16();

				// Bounding Box
				glyph.xMin = p.i16();
				glyph.yMin = p.i16();
				glyph.xMax = p.i16();
				glyph.yMax = p.i16();

				if (glyph.noOfContours >= 0) { // Simple Glpyh
					SimpleGlyph sGlyph;

					SimpleGlyph::Outline outline;

					// contours
					outline.endPtsOfContours = p.vec<uint16_t>(glyph.noOfContours);
					for (auto& c : outline.endPtsOfContours)
						c = p.u16();

					// instructions
					outline.instructionLength = p.u16();
					outline.instructions = p.vec<uint8_t>(outline.instructionLength);
					for (auto& ist : outline.instructions)
						ist = p.u8();

					// flags
					//outline.flags = p.vec<Glyph::Flag>(outline.endPtsOfContours.back() + 1);
					for (int j = 0; j < outline.endPtsOfContours.back() + 1; j++) {
						auto byte = p.u8();
						SimpleGlyph::Flag flag;
						flag.offCurve = byte & 0x01;
						flag.xShort = byte & 0x02;
						flag.yShort = byte & 0x04;
						flag.repeat = byte & 0x08;
						flag.xDual = byte & 0x10;
						flag.yDual = byte & 0x20;
						outline.flags.push_back(flag);

						if (flag.repeat) {
							auto repeatCount = p.u8();
							while (repeatCount--) {
								outline.flags.push_back(flag);
								j++;
							}
						}
					}

					// coordinates
					for (int i = 0; i < outline.flags.size(); i++) {
						auto flag = outline.flags[i];

						if (flag.xShort && flag.xDual) // 1 byte +
							outline.xCoords.push_back(abs((int16_t)p.u8()));
						else if (flag.xShort && !flag.xDual)// 1 byte -
							outline.xCoords.push_back(-abs((int16_t)p.u8()));
						else if (!flag.xShort && flag.xDual) // same as previous
							outline.xCoords.push_back(0);
						else if (!flag.xShort && !flag.xDual) // same as previous
							outline.xCoords.push_back(p.i16());
					}

					for (int i = 0; i < outline.flags.size(); i++) {
						auto flag = outline.flags[i];

						if (flag.yShort && flag.yDual) // 1 byte +
							outline.yCoords.push_back(abs((int16_t)p.u8()));
						else if (flag.yShort && !flag.yDual)// 1 byte -
							outline.yCoords.push_back(-abs((int16_t)p.u8()));
						else if (!flag.yShort && flag.yDual) // same as previous
							outline.yCoords.push_back(0);
						else if (!flag.yShort && !flag.yDual) // same as previous
							outline.yCoords.push_back(p.i16());
					}

					sGlyph.outline = outline;
					glyph.index = simpleGlyphs.size();
					simpleGlyphs.push_back(sGlyph);
				}
				else { // Compound Glyph
					CompoundGlyph cGlyph;
					glyph.index = compoundGlyphs.size();
					compoundGlyphs.push_back(cGlyph);
				}
			}
		}
	};

	struct tbl_cmap {
		uint16_t version;
		uint16_t numTables; // Number of encoding tables that follow.

		struct EncodingRecord {
			uint16_t platformID; // Platform ID, 0 Unicode, 1 mac, 2 iso(deprec), 3 windows, 4 custom
			uint16_t encodingID; // Platform-specific encoding ID( 0 uni 1.0, 1 uni 1.1, 2 iso/iec, 3 unicode 2.0 bmp only, 4 unicode 2.0+ full, 5 unicode var sequence, 6 unicode full rep)
			uint32_t subtableOffset; // Byte offset from beginning of table to the subtable for this encoding.
		};
		std::vector<EncodingRecord> records;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			version = p.u16();
			numTables = p.u16();
			records = p.vec<EncodingRecord>(numTables);
			for (auto& record : records)
				record.platformID = p.u16(), record.encodingID = p.u16(), record.subtableOffset = p.u32();
		}
	};

	struct Format4 {
		uint16_t format; // Format number is set to 4.
		uint16_t length; // This is the length in bytes of the subtable.
		uint16_t language;
		uint16_t segCountX2; //  2 × segCount;
		uint16_t searchRange; //	Maximum power of 2 less than or equal to segCount, times 2 ((2 * *floor(log2(segCount))) * 2, where “ * *” is an exponentiation operator)
		uint16_t entrySelector;// Log2 of the maximum power of 2 less than or equal to segCount(log2(searchRange / 2), which is equal to floor(log2(segCount)))
		uint16_t rangeShift; //	segCount times 2, minus searchRange((segCount * 2) - searchRange)
		std::vector<uint16_t> endCode; //[segCount]	End characterCode for each segment, last = 0xFFFF.
		uint16_t reservedPad; //	Set to 0.
		std::vector<uint16_t> startCode; //[segCount]	Start character code for each segment.
		std::vector<int16_t>  idDelta; //[segCount]	Delta for all character codes in segment.
		std::vector<uint16_t> idRangeOffsets; //[segCount]	Offsets into glyphIdArray or 0
		std::vector<uint16_t> glyphIdArray; //	Glyph index array(arbitrary length)
		std::vector<std::pair<uint16_t, uint16_t>> glyphMap;

		uint16_t retrieve_glyf(int16_t letter) {
			uint16_t index = UINT16_MAX;
			for (auto pr : glyphMap) {
				if (pr.first == letter)
					index = pr.second;
			}
			return index;
		}

		void parse(Parser& p, uint32_t offset, uint32_t length) {
			auto a1 = p.rd_position();

			format = p.u16();
			if (format == 4) {

				std::cout << "Is Format 4\n";
			}
			else {
				throw "Unsupported format : ${format}. Required : 4.";
			}

			length = p.u16();
			language = p.u16();
			segCountX2 = p.u16();
			searchRange = p.u16();
			entrySelector = p.u16();
			rangeShift = p.u16();

			endCode = p.vec<uint16_t>(segCountX2 / 2);
			for (auto& ec : endCode)
				ec = p.u16();

			// resever pad
			reservedPad = p.u16();

			startCode = p.vec<uint16_t>(segCountX2 / 2);
			for (auto& sc : startCode)
				sc = p.u16();

			idDelta = p.vec<int16_t>(segCountX2 / 2);
			for (auto& id : idDelta)
				id = p.i16();

			auto idRangeOffsetStart = p.rd_position();
			idRangeOffsets = p.vec<uint16_t>(segCountX2 / 2);
			for (auto& idRangeOffset : idRangeOffsets)
				idRangeOffset = p.u16();

			auto offset2 = p.rd_position();

			auto remBytes = length - (offset2 - a1);
			glyphIdArray = p.vec<uint16_t>(2255);
			for (int i = 0; i < (remBytes / 2); i++) {
				glyphIdArray[i] = p.u16();
			}

			for (auto i = 0; i < (segCountX2 / 2) - 1; i++) {
				auto glyphIndex = 0;
				auto const i_endCode = endCode[i];
				auto const i_startCode = startCode[i];
				auto const i_idDelta = idDelta[i];
				auto const i_idRangeOffset = idRangeOffsets[i];

				for (auto c = i_startCode; c <= i_endCode; c++) {
					if (i_idRangeOffset != 0) {
						auto const startCodeOffset = (c - i_startCode) * 2;
						auto const currentRangeOffset = i * 2; // 2 because the numbers are 2 byte big.

						auto glyphIndexOffset =
							idRangeOffsetStart + // where all offsets started
							currentRangeOffset + // offset for the current range
							i_idRangeOffset + // offset between the id range table and the glyphIdArray[]
							startCodeOffset; // gets us finally to the character

						p.set_position(glyphIndexOffset);
						glyphIndex = p.u16();
						if (glyphIndex != 0) {
							// & 0xffff is modulo 65536.
							glyphIndex = (glyphIndex + i_idDelta) & 0xffff;
						}
					}
					else {
						glyphIndex = (c + i_idDelta) & 0xffff;
					}

					//glyphMap[i][c] = glyphIndex;
					glyphMap.push_back({ c, glyphIndex });
				}
			}
		}
	};

	struct Coord { float x, y; };

	struct FontGlyph {
		int16_t left, top, right, bottom;
		int16_t lsb, rsb, advance;
		std::vector<std::vector<Coord>> outline;
		std::vector<detail::tbl_glyf::SimpleGlyph::Flag> flags;
	};

	struct FontMetric {
		uint16_t unitsPerEm;

		auto operator ()(char c) {
			return glyphs.at(c);
		}

		std::unordered_map<char, FontGlyph> glyphs;
	};

	auto make_outline(FontGlyph const& glyph) {
		std::vector<std::vector<Coord>> scaledOutline{};
		int step = 0;
		for (int i = 0; i < glyph.outline.size(); i++) {
			auto const& contour = glyph.outline[i];
			std::vector<Coord> scaledContour;



			for (int j = 0; j < contour.size(); j++) {
				auto flag = glyph.flags[step++];

				if (flag.offCurve) {
					if (j < contour.size() - 1) {
						scaledContour.push_back(Coord{ contour[j].x  , contour[j].y });
					}
				}
				else {
					auto curve_points = [&](int startIndex, int controlIndex, int endIndex) {
						auto start = contour[startIndex], control = contour[controlIndex], end = contour[endIndex];
						float t = (1.0 / 3.0);
						for (int i = 0; i < 3; i++) {
							auto dx = start.x + (t * (control.x - start.x)), dy = start.y + (t * (control.y - start.y));
							auto ex = control.x + (t * (end.x - control.x)), ey = control.y + (t * (end.y - control.y));
							auto px = dx + (t * (ex - dx)), py = dy + (t * (ey - dy));
							scaledContour.push_back(Coord{ px , py });
							t += t;
						}
						};
					if (j < contour.size() - 3)
						curve_points(j, j + 1, j + 2);
					if (j == contour.size() - 2)
						curve_points(j, j + 1, 0);
				}
			}
			scaledOutline.push_back(scaledContour);
		}
		return scaledOutline;
	}


}

namespace ttf {
	using Coord = detail::Coord;
	using FontMetric = detail::FontMetric;
	using FontGlyph = detail::FontGlyph;

	std::string alphabet = " !\"#$%&\'()*+,-.\/0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

	auto load_font(std::string const& path) {
		FontMetric metric;

		auto offset = [](detail::tbl_directory& dir, std::string name) {
			return dir.tableRecords.at(name).offset;
			};
		auto length = [](detail::tbl_directory& dir, std::string name) {
			return dir.tableRecords.at(name).length;
			};

		detail::Parser parser{ path };
		detail::tbl_directory directory;
		detail::tbl_head head;
		detail::tbl_name name;
		detail::tbl_os2 os2;
		detail::tbl_cvt cvt;
		detail::tbl_kern kern;
		detail::tbl_maxp maxp;
		detail::tbl_hhea hhea;
		detail::tbl_hmtx hmtx;
		detail::tbl_loca loca;
		detail::tbl_glyf glyf;
		detail::tbl_cmap cmap;
		detail::Format4 fmt4;

		directory.parse(parser);
		head.parse(parser, offset(directory, "head"));
		name.parse(parser, offset(directory, "name"));
		os2.parse(parser, offset(directory, "OS/2"));
		cvt.parse(parser, offset(directory, "cvt "), length(directory, "cvt "));
		kern.parse(parser, offset(directory, "kern"));
		maxp.parse(parser, offset(directory, "maxp"));
		hhea.parse(parser, offset(directory, "hhea"));
		hmtx.parse(hhea.numOfLongHorMetrics, parser, offset(directory, "hmtx"));
		loca.parse(head.indexToLocFormat, maxp.numGlyphs, parser, offset(directory, "loca"));
		glyf.parse(maxp.numGlyphs, loca.offsets, parser, offset(directory, "glyf"));
		cmap.parse(parser, offset(directory, "cmap"));
		fmt4.parse(parser, offset(directory, "cmap"), length(directory, "cmap"));



		for (auto letter : alphabet) {
			FontGlyph glyph;

			uint16_t index = fmt4.retrieve_glyf(letter);
			auto& const glyfA = glyf.glyphs[index];
			auto& const hmtxA = hmtx.entries[index];

			// Glyph Bounding Box
			glyph.left = glyfA.xMin;
			glyph.top = glyfA.yMin;
			glyph.right = glyfA.xMax;
			glyph.bottom = glyfA.yMax;
			glyph.lsb = hmtxA.leftSideBearing;
			glyph.advance = hmtxA.advanceWidth;
			glyph.rsb = (hmtxA.advanceWidth - hmtxA.leftSideBearing - (glyfA.xMax - glyfA.xMin));

			// Glyph Outline
			float xAbs = 0, yAbs = 0;
			auto simple_glyph = glyf.simpleGlyphs[glyf.glyphs[index].index];
			auto glyph_header = glyf.glyphs[index];
			int step = 0;
			for (int i = 0; i < glyph_header.noOfContours; i++) {
				std::vector<Coord> contour;
				auto end = simple_glyph.outline.endPtsOfContours[i];
				for (int j = step; j <= end; j++) {
					xAbs += simple_glyph.outline.xCoords[step], yAbs += simple_glyph.outline.yCoords[step];
					contour.push_back(Coord{ xAbs, yAbs });
					step++;
				}
				glyph.outline.push_back(contour);
			};
			glyph.flags = simple_glyph.outline.flags;
			metric.glyphs.try_emplace(letter, glyph);
		}

		metric.unitsPerEm = head.unitsPerEm;
		return metric;
	}

	void print_glyph(FontGlyph const& glyph, char pos = '*', char neg = ' ') {
		std::string charStr;
		for (int j = glyph.top; j <= glyph.bottom; j++) {
			for (int i = glyph.left; i <= glyph.right; i++) {
				bool hatched = false;
				for (auto& contour : glyph.outline) {
					for (auto p : contour)
						hatched = (p.x == i && j == p.y) ? true : hatched;
				}
				charStr += (hatched) ? pos : neg;
			}
			charStr += '\n';
		}
		std::reverse(charStr.begin(), charStr.end());
		std::cout << charStr << std::endl;
		std::cout << "------------------------------\n";
	}

	auto glyph_scaled(FontMetric const& fm, char unicode, uint16_t size) {
		float scale = (1.0f / fm.unitsPerEm) * size;
		FontGlyph const& glyph = fm.glyphs.at(unicode);
		FontGlyph scaled = glyph;
		scaled.left *= scale;
		scaled.top *= scale;
		scaled.right *= scale;
		scaled.bottom *= scale;
		scaled.lsb *= scale;
		scaled.rsb *= scale;
		scaled.advance *= scale;

		scaled.outline = detail::make_outline(glyph);


		int i = 0, j = 0, k = 0;
		for (auto& contour : scaled.outline)
			for (auto& p : contour)
				p.x *= scale, p.y *= scale;

		// If any points are to close together erase them

		return scaled;
	}

	auto glyph_bitmap(FontMetric const& fm, char c, size_t size) {
		auto cccw = [](Coord A, Coord B, Coord C) {
			return (C.y - A.y) * (B.x - A.x) >= (B.y - A.y) * (C.x - A.x);
			};

		auto intersects = [&](Coord A, Coord B, Coord C, Coord D) {
			return cccw(A, C, D) != cccw(B, C, D) && cccw(A, B, C) != cccw(A, B, D);
			};



		auto glyph = fm.glyphs.at(c);
		glyph.outline = detail::make_outline(glyph);

		glyph = glyph_scaled(fm, c, size);
		//print_glyph(glyph);

		int pw = glyph.right - glyph.left + 5, ph = glyph.bottom - glyph.top + 1;
		std::vector<unsigned char> buffer(pw * ph, unsigned char{});

		for (int16_t j = 0; j < ph; j++) {
			for (int16_t i = 0; i < pw; i++) {
				int modCount = 0;
				for (auto& contour : glyph.outline) {
					for (int k = 0; k < contour.size() - 1; k++) {
						modCount += (intersects(Coord{ 0,  (float)j }, Coord{ (float)i,  (float)j }, contour[k], contour[k + 1])) ? 1 : 0;
					}
					modCount += (intersects(Coord{ (float)glyph.left,  (float)j }, Coord{ (float)i,  (float)j }, contour[0], contour[contour.size() - 1])) ? 1 : 0;
				}
				buffer[(pw * j) + i] = (modCount % 2 != 0) ? 255 : 0;
			}
		}

		// Add antialisanting to bitmap
		auto aa = [&](int16_t x, int16_t y) {
			bool hasLeft = (x > 0), hasRight = (x < pw - 1), hasTop = (y < 0), hasBottom = (y < ph - 1);
			auto at = [&](int16_t xx, int16_t yy) { return buffer[(pw * yy) + xx]; };

			if (at(x, y) == 255)
				return uint8_t(255);

			float blackCount = 0, lvl = 1;
			if (hasLeft)
				blackCount += at(x - 1, y), lvl++;
			if (hasLeft && hasTop)
				blackCount += at(x - 1, y - 1), lvl++;
			if (hasTop)
				blackCount += at(x, y - 1), lvl++;
			if (hasRight && hasTop)
				blackCount += at(x + 1, y - 1), lvl++;
			if (hasRight)
				blackCount += at(x + 1, y);
			if (hasBottom && hasRight)
				blackCount += at(x + 1, y + 1), lvl++;
			if (hasBottom)
				blackCount += at(x, y + 1), lvl++;
			if (hasLeft && hasBottom)
				blackCount += at(x - 1, y + 1), lvl++;

			blackCount += at(x, y);

			auto b = blackCount / lvl;
			b = b < 0 ? 0 : b > 255 ? 255 : b;

			return uint8_t(at(x, y) + b);
			};

		for (int16_t i = 0; i < pw; i++) {
			for (int16_t j = 0; j < ph; j++) {
				buffer[(pw * j) + i] = aa(i, j);
			}
		}

		return std::make_tuple(buffer, pw, ph);
	}

	struct FontAtlas {
		static void Load(std::string const& path, int size) {
			FontMetric fm = load_font(path);

			if (font_map().count(path.c_str()) == 0)
				font_map().try_emplace(path.c_str(), std::unordered_map<int, std::unordered_map<char, AtlasGlyph>>{});

			if (font_map().at(path.c_str()).count(size) > 0)
				return;

			if (font_map().at(path.c_str()).count(size) == 0)
				font_map().at(path.c_str()).try_emplace(size, std::unordered_map<char, AtlasGlyph>{});


			auto& glyph_map = font_map().at(path.c_str()).at(size);
			for (int i = 0; i < alphabet.size(); i++) {
				auto tup = glyph_bitmap(fm, alphabet[i], size);
				AtlasGlyph glyph;
				glyph.glyph = fm.glyphs[alphabet[i]];
				glyph.buffer = std::get<0>(tup);
				glyph.w = std::get<1>(tup), glyph.h = std::get<2>(tup);
				glyph_map.try_emplace(alphabet[i], glyph);
			}

			//font_map().try_emplace(path.c_str(), glyph_map);
		}

		static auto& glyph(std::string font, char c, int size) {
			return font_map().at(font.c_str()).at(size).at(c);
		}

	private:
		struct AtlasGlyph {
			std::vector<unsigned char> buffer;
			int w, h;
			FontGlyph glyph;
		};

		static std::unordered_map<const char*, std::unordered_map<int, std::unordered_map<char, AtlasGlyph>>>& font_map() {
			static std::unordered_map<const char*, std::unordered_map<int, std::unordered_map<char, AtlasGlyph>>> mFonts;
			return mFonts;
		}
	};
}

#endif TRETYPE5