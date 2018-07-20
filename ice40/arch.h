/*
 *  nextpnr -- Next Generation Place and Route
 *
 *  Copyright (C) 2018  Clifford Wolf <clifford@symbioticeda.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef NEXTPNR_H
#error Include "arch.h" via "nextpnr.h" only.
#endif

NEXTPNR_NAMESPACE_BEGIN

/**** Everything in this section must be kept in sync with chipdb.py ****/

template <typename T> struct RelPtr
{
    int32_t offset;

    // void set(const T *ptr) {
    //     offset = reinterpret_cast<const char*>(ptr) -
    //              reinterpret_cast<const char*>(this);
    // }

    const T *get() const { return reinterpret_cast<const T *>(reinterpret_cast<const char *>(this) + offset); }

    const T &operator[](size_t index) const { return get()[index]; }

    const T &operator*() const { return *(get()); }

    const T *operator->() const { return get(); }
};

NPNR_PACKED_STRUCT(struct BelWirePOD {
    int32_t wire_index;
    PortPin port;
});

NPNR_PACKED_STRUCT(struct BelInfoPOD {
    RelPtr<char> name;
    BelType type;
    int32_t num_bel_wires;
    RelPtr<BelWirePOD> bel_wires;
    int8_t x, y, z;
    int8_t padding_0;
});

NPNR_PACKED_STRUCT(struct BelPortPOD {
    int32_t bel_index;
    PortPin port;
});

NPNR_PACKED_STRUCT(struct PipInfoPOD {
    // RelPtr<char> name;
    int32_t src, dst;
    int32_t delay;
    int8_t x, y;
    int16_t src_seg, dst_seg;
    int16_t switch_mask;
    int32_t switch_index;
});

NPNR_PACKED_STRUCT(struct WireSegmentPOD {
    int8_t x, y;
    int16_t index;
});

NPNR_PACKED_STRUCT(struct WireInfoPOD {
    RelPtr<char> name;
    int32_t num_uphill, num_downhill;
    RelPtr<int32_t> pips_uphill, pips_downhill;

    int32_t num_bels_downhill;
    BelPortPOD bel_uphill;
    RelPtr<BelPortPOD> bels_downhill;

    int32_t num_segments;
    RelPtr<WireSegmentPOD> segments;

    int8_t x, y;
    WireType type;
    int8_t padding_0;
});

NPNR_PACKED_STRUCT(struct PackagePinPOD {
    RelPtr<char> name;
    int32_t bel_index;
});

NPNR_PACKED_STRUCT(struct PackageInfoPOD {
    RelPtr<char> name;
    int32_t num_pins;
    RelPtr<PackagePinPOD> pins;
});

enum TileType : uint32_t
{
    TILE_NONE = 0,
    TILE_LOGIC = 1,
    TILE_IO = 2,
    TILE_RAMB = 3,
    TILE_RAMT = 4,
    TILE_DSP0 = 5,
    TILE_DSP1 = 6,
    TILE_DSP2 = 7,
    TILE_DSP3 = 8,
    TILE_IPCON = 9
};

NPNR_PACKED_STRUCT(struct ConfigBitPOD { int8_t row, col; });

NPNR_PACKED_STRUCT(struct ConfigEntryPOD {
    RelPtr<char> name;
    int32_t num_bits;
    RelPtr<ConfigBitPOD> bits;
});

NPNR_PACKED_STRUCT(struct TileInfoPOD {
    int8_t cols, rows;
    int16_t num_config_entries;
    RelPtr<ConfigEntryPOD> entries;
});

static const int max_switch_bits = 5;

NPNR_PACKED_STRUCT(struct SwitchInfoPOD {
    int32_t num_bits;
    int8_t x, y;
    ConfigBitPOD cbits[max_switch_bits];
});

NPNR_PACKED_STRUCT(struct IerenInfoPOD {
    int8_t iox, ioy, ioz;
    int8_t ierx, iery, ierz;
});

NPNR_PACKED_STRUCT(struct BitstreamInfoPOD {
    int32_t num_switches, num_ierens;
    RelPtr<TileInfoPOD> tiles_nonrouting;
    RelPtr<SwitchInfoPOD> switches;
    RelPtr<IerenInfoPOD> ierens;
});

NPNR_PACKED_STRUCT(struct BelConfigEntryPOD {
    RelPtr<char> entry_name;
    RelPtr<char> cbit_name;
    int8_t x, y;
    int16_t padding;
});

// Stores mapping between bel parameters and config bits,
// for extra cells where this mapping is non-trivial
NPNR_PACKED_STRUCT(struct BelConfigPOD {
    int32_t bel_index;
    int32_t num_entries;
    RelPtr<BelConfigEntryPOD> entries;
});

NPNR_PACKED_STRUCT(struct ChipInfoPOD {
    int32_t width, height;
    int32_t num_bels, num_wires, num_pips;
    int32_t num_switches, num_belcfgs, num_packages;
    RelPtr<BelInfoPOD> bel_data;
    RelPtr<WireInfoPOD> wire_data;
    RelPtr<PipInfoPOD> pip_data;
    RelPtr<TileType> tile_grid;
    RelPtr<BitstreamInfoPOD> bits_info;
    RelPtr<BelConfigPOD> bel_config;
    RelPtr<PackageInfoPOD> packages_data;
});

#if defined(_MSC_VER)
extern const char *chipdb_blob_384;
extern const char *chipdb_blob_1k;
extern const char *chipdb_blob_5k;
extern const char *chipdb_blob_8k;
#else
extern const char chipdb_blob_384[];
extern const char chipdb_blob_1k[];
extern const char chipdb_blob_5k[];
extern const char chipdb_blob_8k[];
#endif

/************************ End of chipdb section. ************************/

struct BelIterator
{
    int cursor;

    BelIterator operator++()
    {
        cursor++;
        return *this;
    }
    BelIterator operator++(int)
    {
        BelIterator prior(*this);
        cursor++;
        return prior;
    }

    bool operator!=(const BelIterator &other) const { return cursor != other.cursor; }

    bool operator==(const BelIterator &other) const { return cursor == other.cursor; }

    BelId operator*() const
    {
        BelId ret;
        ret.index = cursor;
        return ret;
    }
};

struct BelRange
{
    BelIterator b, e;
    BelIterator begin() const { return b; }
    BelIterator end() const { return e; }
};

// -----------------------------------------------------------------------

struct BelPinIterator
{
    const BelPortPOD *ptr = nullptr;

    void operator++() { ptr++; }
    bool operator!=(const BelPinIterator &other) const { return ptr != other.ptr; }

    BelPin operator*() const
    {
        BelPin ret;
        ret.bel.index = ptr->bel_index;
        ret.pin = ptr->port;
        return ret;
    }
};

struct BelPinRange
{
    BelPinIterator b, e;
    BelPinIterator begin() const { return b; }
    BelPinIterator end() const { return e; }
};

// -----------------------------------------------------------------------

struct WireIterator
{
    int cursor = -1;

    void operator++() { cursor++; }
    bool operator!=(const WireIterator &other) const { return cursor != other.cursor; }

    WireId operator*() const
    {
        WireId ret;
        ret.index = cursor;
        return ret;
    }
};

struct WireRange
{
    WireIterator b, e;
    WireIterator begin() const { return b; }
    WireIterator end() const { return e; }
};

// -----------------------------------------------------------------------

struct AllPipIterator
{
    int cursor = -1;

    void operator++() { cursor++; }
    bool operator!=(const AllPipIterator &other) const { return cursor != other.cursor; }

    PipId operator*() const
    {
        PipId ret;
        ret.index = cursor;
        return ret;
    }
};

struct AllPipRange
{
    AllPipIterator b, e;
    AllPipIterator begin() const { return b; }
    AllPipIterator end() const { return e; }
};

// -----------------------------------------------------------------------

struct PipIterator
{
    const int *cursor = nullptr;

    void operator++() { cursor++; }
    bool operator!=(const PipIterator &other) const { return cursor != other.cursor; }

    PipId operator*() const
    {
        PipId ret;
        ret.index = *cursor;
        return ret;
    }
};

struct PipRange
{
    PipIterator b, e;
    PipIterator begin() const { return b; }
    PipIterator end() const { return e; }
};

struct ArchArgs
{
    enum ArchArgsTypes
    {
        NONE,
        LP384,
        LP1K,
        LP8K,
        HX1K,
        HX8K,
        UP5K
    } type = NONE;
    std::string package;
};

struct Arch : BaseCtx
{
    const ChipInfoPOD *chip_info;
    const PackageInfoPOD *package_info;

    mutable std::unordered_map<IdString, int> bel_by_name;
    mutable std::unordered_map<IdString, int> wire_by_name;
    mutable std::unordered_map<IdString, int> pip_by_name;

    std::vector<IdString> bel_to_cell;
    std::vector<IdString> wire_to_net;
    std::vector<IdString> pip_to_net;
    std::vector<IdString> switches_locked;

    ArchArgs args;
    Arch(ArchArgs args);

    std::string getChipName();

    IdString archId() const { return id("ice40"); }
    IdString archArgsToId(ArchArgs args) const;

    IdString belTypeToId(BelType type) const;
    BelType belTypeFromId(IdString id) const;

    IdString portPinToId(PortPin type) const;
    PortPin portPinFromId(IdString id) const;

    // -------------------------------------------------

    BelId getBelByName(IdString name) const;

    IdString getBelName(BelId bel) const
    {
        NPNR_ASSERT(bel != BelId());
        return id(chip_info->bel_data[bel.index].name.get());
    }

    uint32_t getBelChecksum(BelId bel) const { return bel.index; }

    void bindBel(BelId bel, IdString cell, PlaceStrength strength)
    {
        NPNR_ASSERT(bel != BelId());
        NPNR_ASSERT(bel_to_cell[bel.index] == IdString());
        bel_to_cell[bel.index] = cell;
        cells[cell]->bel = bel;
        cells[cell]->belStrength = strength;
    }

    void unbindBel(BelId bel)
    {
        NPNR_ASSERT(bel != BelId());
        NPNR_ASSERT(bel_to_cell[bel.index] != IdString());
        cells[bel_to_cell[bel.index]]->bel = BelId();
        cells[bel_to_cell[bel.index]]->belStrength = STRENGTH_NONE;
        bel_to_cell[bel.index] = IdString();
    }

    bool checkBelAvail(BelId bel) const
    {
        NPNR_ASSERT(bel != BelId());
        return bel_to_cell[bel.index] == IdString();
    }

    IdString getBoundBelCell(BelId bel) const
    {
        NPNR_ASSERT(bel != BelId());
        return bel_to_cell[bel.index];
    }

    IdString getConflictingBelCell(BelId bel) const
    {
        NPNR_ASSERT(bel != BelId());
        return bel_to_cell[bel.index];
    }

    BelRange getBels() const
    {
        BelRange range;
        range.b.cursor = 0;
        range.e.cursor = chip_info->num_bels;
        return range;
    }

    BelRange getBelsByType(BelType type) const
    {
        BelRange range;
// FIXME
#if 0
		if (type == "TYPE_A") {
			range.b.cursor = bels_type_a_begin;
			range.e.cursor = bels_type_a_end;
		}
		...
#endif
        return range;
    }

    Loc getBelLocation(BelId bel) const
    {
        Loc loc;
        loc.x = chip_info->bel_data[bel.index].x;
        loc.y = chip_info->bel_data[bel.index].y;
        loc.z = chip_info->bel_data[bel.index].z;
        return loc;
    }

    BelId getBelByLocation(int x, int y, int z) const
    {
        // FIXME
        return BelId();
    }

    BelRange getBelsByTile(int x, int y) const
    {
        BelRange range;
        // FIXME
        return range;
    }

    bool getBelGlobalBuf(BelId bel) const
    {
        return chip_info->bel_data[bel.index].type == TYPE_SB_GB;
    }

    BelRange getBelsAtSameTile(BelId bel) const NPNR_DEPRECATED;

    BelType getBelType(BelId bel) const
    {
        NPNR_ASSERT(bel != BelId());
        return chip_info->bel_data[bel.index].type;
    }

    WireId getWireBelPin(BelId bel, PortPin pin) const;

    BelPin getBelPinUphill(WireId wire) const
    {
        BelPin ret;
        NPNR_ASSERT(wire != WireId());

        if (chip_info->wire_data[wire.index].bel_uphill.bel_index >= 0) {
            ret.bel.index = chip_info->wire_data[wire.index].bel_uphill.bel_index;
            ret.pin = chip_info->wire_data[wire.index].bel_uphill.port;
        }

        return ret;
    }

    BelPinRange getBelPinsDownhill(WireId wire) const
    {
        BelPinRange range;
        NPNR_ASSERT(wire != WireId());
        range.b.ptr = chip_info->wire_data[wire.index].bels_downhill.get();
        range.e.ptr = range.b.ptr + chip_info->wire_data[wire.index].num_bels_downhill;
        return range;
    }

    // -------------------------------------------------

    WireId getWireByName(IdString name) const;

    IdString getWireName(WireId wire) const
    {
        NPNR_ASSERT(wire != WireId());
        return id(chip_info->wire_data[wire.index].name.get());
    }

    uint32_t getWireChecksum(WireId wire) const { return wire.index; }

    void bindWire(WireId wire, IdString net, PlaceStrength strength)
    {
        NPNR_ASSERT(wire != WireId());
        NPNR_ASSERT(wire_to_net[wire.index] == IdString());
        wire_to_net[wire.index] = net;
        nets[net]->wires[wire].pip = PipId();
        nets[net]->wires[wire].strength = strength;
    }

    void unbindWire(WireId wire)
    {
        NPNR_ASSERT(wire != WireId());
        NPNR_ASSERT(wire_to_net[wire.index] != IdString());

        auto &net_wires = nets[wire_to_net[wire.index]]->wires;
        auto it = net_wires.find(wire);
        NPNR_ASSERT(it != net_wires.end());

        auto pip = it->second.pip;
        if (pip != PipId()) {
            pip_to_net[pip.index] = IdString();
            switches_locked[chip_info->pip_data[pip.index].switch_index] = IdString();
        }

        net_wires.erase(it);
        wire_to_net[wire.index] = IdString();
    }

    bool checkWireAvail(WireId wire) const
    {
        NPNR_ASSERT(wire != WireId());
        return wire_to_net[wire.index] == IdString();
    }

    IdString getBoundWireNet(WireId wire) const
    {
        NPNR_ASSERT(wire != WireId());
        return wire_to_net[wire.index];
    }

    IdString getConflictingWireNet(WireId wire) const
    {
        NPNR_ASSERT(wire != WireId());
        return wire_to_net[wire.index];
    }

    WireRange getWires() const
    {
        WireRange range;
        range.b.cursor = 0;
        range.e.cursor = chip_info->num_wires;
        return range;
    }

    // -------------------------------------------------

    PipId getPipByName(IdString name) const;

    void bindPip(PipId pip, IdString net, PlaceStrength strength)
    {
        NPNR_ASSERT(pip != PipId());
        NPNR_ASSERT(pip_to_net[pip.index] == IdString());
        NPNR_ASSERT(switches_locked[chip_info->pip_data[pip.index].switch_index] == IdString());

        pip_to_net[pip.index] = net;
        switches_locked[chip_info->pip_data[pip.index].switch_index] = net;

        WireId dst;
        dst.index = chip_info->pip_data[pip.index].dst;
        NPNR_ASSERT(wire_to_net[dst.index] == IdString());
        wire_to_net[dst.index] = net;
        nets[net]->wires[dst].pip = pip;
        nets[net]->wires[dst].strength = strength;
    }

    void unbindPip(PipId pip)
    {
        NPNR_ASSERT(pip != PipId());
        NPNR_ASSERT(pip_to_net[pip.index] != IdString());
        NPNR_ASSERT(switches_locked[chip_info->pip_data[pip.index].switch_index] != IdString());

        WireId dst;
        dst.index = chip_info->pip_data[pip.index].dst;
        NPNR_ASSERT(wire_to_net[dst.index] != IdString());
        wire_to_net[dst.index] = IdString();
        nets[pip_to_net[pip.index]]->wires.erase(dst);

        pip_to_net[pip.index] = IdString();
        switches_locked[chip_info->pip_data[pip.index].switch_index] = IdString();
    }

    bool checkPipAvail(PipId pip) const
    {
        NPNR_ASSERT(pip != PipId());
        return switches_locked[chip_info->pip_data[pip.index].switch_index] == IdString();
    }

    IdString getBoundPipNet(PipId pip) const
    {
        NPNR_ASSERT(pip != PipId());
        return pip_to_net[pip.index];
    }

    IdString getConflictingPipNet(PipId pip) const
    {
        NPNR_ASSERT(pip != PipId());
        return switches_locked[chip_info->pip_data[pip.index].switch_index];
    }

    AllPipRange getPips() const
    {
        AllPipRange range;
        range.b.cursor = 0;
        range.e.cursor = chip_info->num_pips;
        return range;
    }

    IdString getPipName(PipId pip) const;

    uint32_t getPipChecksum(PipId pip) const { return pip.index; }

    WireId getPipSrcWire(PipId pip) const
    {
        WireId wire;
        NPNR_ASSERT(pip != PipId());
        wire.index = chip_info->pip_data[pip.index].src;
        return wire;
    }

    WireId getPipDstWire(PipId pip) const
    {
        WireId wire;
        NPNR_ASSERT(pip != PipId());
        wire.index = chip_info->pip_data[pip.index].dst;
        return wire;
    }

    DelayInfo getPipDelay(PipId pip) const
    {
        DelayInfo delay;
        NPNR_ASSERT(pip != PipId());
        delay.delay = chip_info->pip_data[pip.index].delay;
        return delay;
    }

    PipRange getPipsDownhill(WireId wire) const
    {
        PipRange range;
        NPNR_ASSERT(wire != WireId());
        range.b.cursor = chip_info->wire_data[wire.index].pips_downhill.get();
        range.e.cursor = range.b.cursor + chip_info->wire_data[wire.index].num_downhill;
        return range;
    }

    PipRange getPipsUphill(WireId wire) const
    {
        PipRange range;
        NPNR_ASSERT(wire != WireId());
        range.b.cursor = chip_info->wire_data[wire.index].pips_uphill.get();
        range.e.cursor = range.b.cursor + chip_info->wire_data[wire.index].num_uphill;
        return range;
    }

    PipRange getWireAliases(WireId wire) const
    {
        PipRange range;
        NPNR_ASSERT(wire != WireId());
        range.b.cursor = nullptr;
        range.e.cursor = nullptr;
        return range;
    }

    BelId getPackagePinBel(const std::string &pin) const;
    std::string getBelPackagePin(BelId bel) const;

    // -------------------------------------------------

    GroupId getGroupByName(IdString name) const;
    IdString getGroupName(GroupId group) const;
    std::vector<GroupId> getGroups() const;
    std::vector<BelId> getGroupBels(GroupId group) const;
    std::vector<WireId> getGroupWires(GroupId group) const;
    std::vector<PipId> getGroupPips(GroupId group) const;
    std::vector<GroupId> getGroupGroups(GroupId group) const;

    // -------------------------------------------------

    void estimatePosition(BelId bel, int &x, int &y, bool &gb) const NPNR_DEPRECATED;
    delay_t estimateDelay(WireId src, WireId dst) const;
    delay_t getDelayEpsilon() const { return 20; }
    delay_t getRipupDelayPenalty() const { return 200; }
    float getDelayNS(delay_t v) const { return v * 0.001; }
    uint32_t getDelayChecksum(delay_t v) const { return v; }

    // -------------------------------------------------

    bool pack();
    bool place();
    bool route();

    // -------------------------------------------------

    std::vector<GraphicElement> getDecalGraphics(DecalId decal) const;

    DecalXY getFrameDecal() const;
    DecalXY getBelDecal(BelId bel) const;
    DecalXY getWireDecal(WireId wire) const;
    DecalXY getPipDecal(PipId pip) const;
    DecalXY getGroupDecal(GroupId group) const;

    // -------------------------------------------------

    // Get the delay through a cell from one port to another, returning false
    // if no path exists
    bool getCellDelay(const CellInfo *cell, IdString fromPort, IdString toPort, delay_t &delay) const;
    // Get the associated clock to a port, or empty if the port is combinational
    IdString getPortClock(const CellInfo *cell, IdString port) const;
    // Return true if a port is a clock
    bool isClockPort(const CellInfo *cell, IdString port) const;
    // Return true if a port is a net
    bool isGlobalNet(const NetInfo *net) const;

    // -------------------------------------------------

    // Perform placement validity checks, returning false on failure (all implemented in arch_place.cc)

    // Whether or not a given cell can be placed at a given Bel
    // This is not intended for Bel type checks, but finer-grained constraints
    // such as conflicting set/reset signals, etc
    bool isValidBelForCell(CellInfo *cell, BelId bel) const;

    // Return true whether all Bels at a given location are valid
    bool isBelLocationValid(BelId bel) const;

    // Helper function for above
    bool logicCellsCompatible(const std::vector<const CellInfo *> &cells) const;

    // -------------------------------------------------
    // Assign architecure-specific arguments to nets and cells, which must be called between packing or further
    // netlist modifications, and validity checks
    void assignArchInfo();
    void assignCellInfo(CellInfo *cell);

    IdString id_glb_buf_out;
    IdString id_icestorm_lc, id_sb_io, id_sb_gb;
    IdString id_cen, id_clk, id_sr;
    IdString id_i0, id_i1, id_i2, id_i3;
    IdString id_dff_en, id_neg_clk;
};

NEXTPNR_NAMESPACE_END
