pragma solidity ^0.5.0;

import "./Auction.sol";

contract GhostBase {

    event Transfer(address from, address to, uint256 tokenId);
    event Birth(address owner, uint256 tokenId, uint256 gene);
    event LevelUp(address owner, uint256 tokenId, uint256 level);

    struct Ghost {
        // Ghosts have unique genetic code which is packed into 256-bits.
        uint256 gene;

        // timestamp when ghost is born.
        uint64 birthTime;

        // Ghost's level. This is related to ghost's appearance.
        // level = 0 : egg
        // level > 0 : hatched egg
        uint8 level;
    }

    // An array containing ghosts that exist. Index of this array is used as ghostId.
    Ghost[] ghosts;

    // mapping from ghostId to address of owner.
    mapping (uint256 => address) public ghostIndexToOwner;

    // mapping from ghostId to address that has been approved to call transferfrom().
    mapping (uint256 => address) public ghostIndexToApproved;

    // mapping from address of owner to the number of tokens owned by owner.
    mapping (address => uint256) ownershipTokenCount;

    Auction public candidateAuctionContract;

    // @notice Assigning ownership of ghost from _from to _to.
    function _transfer(address _from, address _to, uint256 _tokenId) internal {
        require(_to != address(0));

        ownershipTokenCount[_to]++;
        ghostIndexToOwner[_tokenId] = _to;
        delete ghostIndexToApproved[_tokenId];

        ownershipTokenCount[_from]--;

        emit Transfer(_from, _to, _tokenId);
    }

    // @notice create egg that ghost with level 0.
    //  Given owner owns this egg.
    function _createEgg(uint256 _gene, address _owner) internal returns (uint256) {

        Ghost memory _ghost = Ghost({
            gene: _gene,
            birthTime: uint64(now),
            level: 0
            });

        uint256 newGhostId = ghosts.push(_ghost) - 1;

        // TODO: setting limit of # of ghosts
        // require(newGhostId == uint256(uint32(newGhostId)));

        emit Birth(_owner, newGhostId, _ghost.gene);
        return newGhostId;
    }

    // TODO: distinguish angel and devil
    // @notice distinguish ghost whether angel or devil.
    //  This is used to calculate ratio of angel and devil.
    function _distinguishAngelAndDevil() internal view returns (bool) {
        return false;
    }

    // @notice increase level of ghost.
    function _levelUp(uint256 _tokenId) internal {
        require(_owns(msg.sender, _tokenId));

        // TODO: setting limit of level
        ghosts[_tokenId].level++;
        emit LevelUp(msg.sender, _tokenId, ghosts[_tokenId].level);
    }
}