// 抽象工厂，没有工厂模式的问题。叫做家族工厂可能更合适。

// 数据库访问有关的基类
class IDBConnection {};
class IDBCommand {};
class IDataReader {};
class IDBFactory {
 public:
  virtual IDBConnection* CreateDBConnection() = 0;
  virtual IDBCommand* CreateDBCommand() = 0;
  virtual IDataReader* CreateDataReader() = 0;
};

//支持SQL Server
class SqlConnection : public IDBConnection {};
class SqlCommand : public IDBCommand {};
class SqlDataReader : public IDataReader {};
class SqlDBFactory : public IDBFactory {
 public:
  virtual IDBConnection* CreateDBConnection() = 0;
  virtual IDBCommand* CreateDBCommand() = 0;
  virtual IDataReader* CreateDataReader() = 0;
};

//支持Oracle
class OracleConnection : public IDBConnection {};
class OracleCommand : public IDBCommand {};
class OracleDataReader : public IDataReader {};
class OracleDBFactory : public IDBFactory {
 public:
  virtual OracleConnection* CreateDBConnection() = 0;
  virtual OracleCommand* CreateDBCommand() = 0;
  virtual OracleDataReader* CreateDataReader() = 0;
};

class EmployeeDAO {
  IDBFactory* dbFactory;

 public:
  vector<EmployeeDO> GetEmployees() {
    IDBConnection* connection = dbFactory->CreateDBConnection();
    connection->ConnectionString("...");

    IDBCommand* command = dbFactory->CreateDBCommand();
    command->CommandText("...");
    command->SetConnection(connection);  // 关联性

    IDBDataReader* reader = dbFactory->CreateDataReader();
    reader->ExecuteReader();  // 关联性
    while (reader->Read()) {
    }
  }
};
