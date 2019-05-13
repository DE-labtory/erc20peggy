pragma solidity ^0.5.0;

import "./GhostOwnership.sol";

contract GhostFactory is GhostOwnership {

    // TODO: setting authority of calling this method
    // @notice create egg that ghost with level 0.
    //  Must manage ghost ownership.
    function createEgg(uint256 _gene, address _owner) external {
        require(_gene != uint256(0));
        require(_owner != address(0));

        uint256 tokenId = _createEgg(_gene, _owner);

        ghostIndexToOwner[tokenId] = _owner;
        ownershipTokenCount[tokenId]++;
    }

    // TODO: calculate ratio of angel and devil
    // @notice returns ratio of angel and devil.
    function getGauge() public view returns (uint, uint) {
        return (0, 0);
    }

    // @notice increase level of ghost.
    function levelUp(uint256 _tokenId) external {
        _levelUp(_tokenId);
    }
}