#pragma once

#include <optional>
#include <Core/NamesAndTypes.h>
#include <Storages/IStorage_fwd.h>
#include <Storages/StorageInMemoryMetadata.h>
#include <Storages/ColumnsDescription.h>

#include <Common/SettingsChanges.h>


namespace DB
{

class ASTAlterCommand;

/// Operation from the ALTER query (except for manipulation with PART/PARTITION).
/// Adding Nested columns is not expanded to add individual columns.
struct AlterCommand
{
    enum Type
    {
        ADD_COLUMN,
        DROP_COLUMN,
        MODIFY_COLUMN,
        COMMENT_COLUMN,
        MODIFY_ORDER_BY,
        ADD_INDEX,
        DROP_INDEX,
        ADD_CONSTRAINT,
        DROP_CONSTRAINT,
        MODIFY_TTL,
        MODIFY_SETTING,
        MODIFY_QUERY,
    };

    Type type;

    String column_name;

    /// For DROP COLUMN ... FROM PARTITION
    String partition_name;

    /// For ADD and MODIFY, a new column type.
    DataTypePtr data_type = nullptr;

    ColumnDefaultKind default_kind{};
    ASTPtr default_expression{};

    /// For COMMENT column
    std::optional<String> comment;

    /// For ADD - after which column to add a new one. If an empty string, add to the end. To add to the beginning now it is impossible.
    String after_column;

    /// For DROP_COLUMN, MODIFY_COLUMN, COMMENT_COLUMN
    bool if_exists = false;

    /// For ADD_COLUMN
    bool if_not_exists = false;

    /// For MODIFY_ORDER_BY
    ASTPtr order_by = nullptr;

    /// For ADD INDEX
    ASTPtr index_decl = nullptr;
    String after_index_name;

    /// For ADD/DROP INDEX
    String index_name;

    // For ADD CONSTRAINT
    ASTPtr constraint_decl = nullptr;

    // For ADD/DROP CONSTRAINT
    String constraint_name;

    /// For MODIFY TTL
    ASTPtr ttl = nullptr;

    /// indicates that this command should not be applied, for example in case of if_exists=true and column doesn't exist.
    bool ignore = false;

    /// For ADD and MODIFY
    CompressionCodecPtr codec = nullptr;

    /// For MODIFY SETTING
    SettingsChanges settings_changes;

    /// For MODIFY_QUERY
    ASTPtr select = nullptr;

    static std::optional<AlterCommand> parse(const ASTAlterCommand * command);

    void apply(StorageInMemoryMetadata & metadata) const;

    /// Checks that alter query changes data. For MergeTree:
    ///    * column files (data and marks)
    ///    * each part meta (columns.txt)
    /// in each part on disk (it's not lightweight alter).
    bool isModifyingData() const;

    /// Checks that only settings changed by alter
    bool isSettingsAlter() const;

    /// Checks that only comment changed by alter
    bool isCommentAlter() const;
};

/// Return string representation of AlterCommand::Type
String alterTypeToString(const AlterCommand::Type type);

class Context;

/// Vector of AlterCommand with several additional functions
class AlterCommands : public std::vector<AlterCommand>
{
private:
    bool prepared = false;
private:

    /// Validate that default expression and type are compatible, i.e. default
    /// expression result can be casted to column_type
    void validateDefaultExpressionForColumn(
        const ASTPtr default_expression,
        const String & column_name,
        const DataTypePtr column_type,
        const ColumnsDescription & all_columns,
        const Context & context) const;

public:
    /// Validate that commands can be applied to metadata.
    /// Checks that all columns exist and dependecies between them.
    /// This check is lightweight and base only on metadata.
    /// More accurate check have to be performed with storage->checkAlterIsPossible.
    void validate(const StorageInMemoryMetadata & metadata, const Context & context) const;

    /// Prepare alter commands. Set ignore flag to some of them and set some
    /// parts to commands from storage's metadata (for example, absent default)
    void prepare(const StorageInMemoryMetadata & metadata);

    /// Apply all alter command in sequential order to storage metadata.
    /// Commands have to be prepared before apply.
    void apply(StorageInMemoryMetadata & metadata) const;

    /// At least one command modify data on disk.
    bool isModifyingData() const;

    /// At least one command modify settings.
    bool isSettingsAlter() const;

    /// At least one command modify comments.
    bool isCommentAlter() const;
};

}
